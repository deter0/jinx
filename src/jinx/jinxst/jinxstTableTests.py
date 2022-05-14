import os
import subprocess

TEST_DEFINITIONS="-DTEST_JINXST_TABLE=1"

def runCommandAndEcho(command: str) -> None:
    print(f"+ {command}")
    process = (subprocess.run(command, shell=True))
    if (process.returncode != 0):
        raise RuntimeError(f"\033[0;31mCommand `{command}` FAILED!")
    pass;

OUT="jinxstTable"

runCommandAndEcho(f"clang -lm -gdbx -Wall -Weverything jinxstTable.c -o {OUT} {TEST_DEFINITIONS}")
runCommandAndEcho(f"./{OUT} +greeting hello +goodbyes cya =goodbyes cya")
runCommandAndEcho(f"rm {OUT}")