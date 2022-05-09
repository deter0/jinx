import os
import subprocess

TEST_DEFINITIONS="-DTEST_JINXST_TABLE=1"

def runCommandAndEcho(command: str) -> None:
    print(f"+ {command}")
    subprocess.run(command, shell=True)
    pass;

OUT="jinxstTable"

runCommandAndEcho(f"clang -lm -gdbx -Wall -Weverything jinxstTable.c -o {OUT} {TEST_DEFINITIONS}")
runCommandAndEcho(f"./{OUT} +greeting hello +goodbyes cya =goodbye cya")
# runCommandAndEcho(f"rm {OUT}")