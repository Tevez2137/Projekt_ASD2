import subprocess
import sys
from pathlib import Path
from time import perf_counter

SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parent
BUILD_DIR = REPO_ROOT / "build"
TARGET = BUILD_DIR / "symulacja_krasnoludkow.exe"
TEST_ROOT = SCRIPT_DIR / "przykladowe"
OUTPUT_CSV = SCRIPT_DIR / "benchmark_results.csv"


def build_binary(rebuild: bool = False) -> bool:
    if not TARGET.exists() or rebuild:
        print("Kompiluję projekt C++ przy użyciu make...")
        proc = subprocess.run(["make"], cwd=REPO_ROOT, capture_output=True, text=True)
        if proc.returncode != 0:
            print("Błąd kompilacji:")
            print(proc.stdout)
            print(proc.stderr)
            return False
        if not TARGET.exists():
            print(f"Nie znaleziono wynikowego pliku binarnego: {TARGET}")
            return False
    return True


def run_command(args, timeout=1800):
    start = perf_counter()
    try:
        proc = subprocess.run(args, cwd=REPO_ROOT, capture_output=True, text=True, timeout=timeout)
        end = perf_counter()
        return proc.returncode, proc.stdout, proc.stderr, end - start
    except subprocess.TimeoutExpired as exc:
        end = perf_counter()
        stderr_text = exc.stderr if exc.stderr is not None else ""
        return -1, exc.stdout if exc.stdout is not None else "", stderr_text + f"\nCommand timed out after {timeout} seconds.", end - start


def find_test_cases():
    cases = []
    if not TEST_ROOT.exists():
        raise FileNotFoundError(f"Brak katalogu testowego: {TEST_ROOT}")
    
    # Szukaj bezpośrednio w folderze testowym (pliki CSV)
    files_dict = {}
    for file in TEST_ROOT.glob("*.csv"):
        name = file.name.lower()
        if "kopalnie" in name:
            # Ekstrahuj nazwę testu z nazwy pliku
            test_name = file.name.replace("kopalnie_", "").replace(".csv", "")
            if test_name not in files_dict:
                files_dict[test_name] = {}
            files_dict[test_name]["kopalnie"] = file
        elif "krasnolud" in name:
            test_name = file.name.replace("krasnoludki_", "").replace(".csv", "")
            if test_name not in files_dict:
                files_dict[test_name] = {}
            files_dict[test_name]["krasnoludki"] = file
    
    # Zbierz pary plików
    for test_name, files in sorted(files_dict.items()):
        if "kopalnie" in files and "krasnoludki" in files:
            cases.append((test_name, files["kopalnie"], files["krasnoludki"]))
    
    # Fallback: szukaj w podfolderach (dla starych struktur)
    if not cases:
        for folder in sorted(TEST_ROOT.iterdir()):
            if not folder.is_dir():
                continue
            kopalnie = None
            krasnoludki = None
            for file in folder.iterdir():
                name = file.name.lower()
                if "kopalnie" in name and file.suffix.lower() == ".csv":
                    kopalnie = file
                elif "krasnolud" in name and file.suffix.lower() == ".csv":
                    krasnoludki = file
            if kopalnie and krasnoludki:
                cases.append((folder.name, kopalnie, krasnoludki))
    
    return cases


def benchmark_case(name, kopalnie, krasnoludki, runs=1):
    print(f"\n== {name} ==")
    result = {
        "case": name,
        "import_time_s": None,
        "dump_time_s": None,
        "salwa_time_s": None,
        "import_ok": False,
        "dump_ok": False,
        "salwa_ok": False,
        "import_stdout": "",
        "dump_stdout": "",
        "salwa_stdout": "",
        "import_stderr": "",
        "dump_stderr": "",
        "salwa_stderr": "",
    }

    # IMPORT
    import_times = []
    for i in range(runs):
        code, out, err, elapsed = run_command([
            str(TARGET),
            "IMPORT",
            str(kopalnie),
            str(krasnoludki),
        ])
        import_times.append(elapsed)
        if i == 0:
            result["import_stdout"] = out
            result["import_stderr"] = err
        if code != 0:
            print(f"IMPORT failed (run {i+1}): returncode={code}")
            result["import_ok"] = False
            break
        result["import_ok"] = True
    if result["import_ok"]:
        result["import_time_s"] = sum(import_times) / len(import_times)
        print(f"IMPORT avg: {result['import_time_s']:.4f}s")

    # GUI_DATA_DUMP
    dump_times = []
    for i in range(runs):
        code, out, err, elapsed = run_command([
            str(TARGET),
            "GUI_DATA_DUMP",
        ])
        dump_times.append(elapsed)
        if i == 0:
            result["dump_stdout"] = out
            result["dump_stderr"] = err
        if code != 0:
            print(f"GUI_DATA_DUMP failed (run {i+1}): returncode={code}")
            result["dump_ok"] = False
            break
        result["dump_ok"] = True
    if result["dump_ok"]:
        result["dump_time_s"] = sum(dump_times) / len(dump_times)
        print(f"GUI_DATA_DUMP avg: {result['dump_time_s']:.4f}s")

    # GUI_DATA_DUMP SALWA 0 0
    salwa_times = []
    for i in range(runs):
        code, out, err, elapsed = run_command([
            str(TARGET),
            "GUI_DATA_DUMP",
            "SALWA",
            "0",
            "0",
        ])
        salwa_times.append(elapsed)
        if i == 0:
            result["salwa_stdout"] = out
            result["salwa_stderr"] = err
        if code != 0:
            print(f"GUI_DATA_DUMP SALWA failed (run {i+1}): returncode={code}")
            result["salwa_ok"] = False
            break
        result["salwa_ok"] = True
    if result["salwa_ok"]:
        result["salwa_time_s"] = sum(salwa_times) / len(salwa_times)
        print(f"GUI_DATA_DUMP SALWA avg: {result['salwa_time_s']:.4f}s")

    return result


def write_csv(results):
    header = [
        "case",
        "import_ok",
        "import_time_s",
        "dump_ok",
        "dump_time_s",
        "salwa_ok",
        "salwa_time_s",
    ]
    lines = [",".join(header)]
    for row in results:
        line = ",".join([
            row["case"],
            str(row["import_ok"]),
            f"{row['import_time_s']:.6f}" if row["import_time_s"] is not None else "",
            str(row["dump_ok"]),
            f"{row['dump_time_s']:.6f}" if row["dump_time_s"] is not None else "",
            str(row["salwa_ok"]),
            f"{row['salwa_time_s']:.6f}" if row["salwa_time_s"] is not None else "",
        ])
        lines.append(line)
    OUTPUT_CSV.write_text("\n".join(lines), encoding="utf-8")
    print(f"Zapisano wyniki do: {OUTPUT_CSV}")


def main():
    rebuild = "--rebuild" in sys.argv
    runs = 1
    for arg in sys.argv[1:]:
        if arg.startswith("--runs="):
            try:
                runs = int(arg.split("=", 1)[1])
            except ValueError:
                pass

    if not build_binary(rebuild=rebuild):
        print("Kompilacja nie powiodła się. Sprawdź komunikaty i spróbuj ponownie.")
        sys.exit(1)

    cases = find_test_cases()
    if not cases:
        print(f"Nie znaleziono przypadków testowych w katalogu {TEST_ROOT}")
        sys.exit(1)

    print(f"Znaleziono {len(cases)} przypadków testowych w: {TEST_ROOT}")
    results = []
    for name, kopalnie, krasnoludki in cases:
        results.append(benchmark_case(name, kopalnie, krasnoludki, runs=runs))

    write_csv(results)


if __name__ == "__main__":
    main()
