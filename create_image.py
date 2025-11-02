import os
import shutil
import argparse
from BodyControlModule.build import execute, build_bcm, BINARY_NAME


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--target',
                        choices=['vm', 'rpi'],
                        default='rpi',
                        help="Build image for: vm or rpi"
                        )
    args = parser.parse_args()
    if not os.getcwd().endswith("Real-Time-OS"):
        print("Run this from the Real-Time-OS directory")
        exit(1)

    print("Building QNX image")

    # Build BodyControlModule for desired platform
    print("Building BodyControlModule")
    os.chdir("BodyControlModule")
    build_bcm(args.target == "rpi", f"{BINARY_NAME}_{args.target}")
    # mkqnximage for that platform
    make_image(args.target == "rpi")

def make_image(rpi: bool):
    mkqnximage_path = shutil.which("mkqnximage")
    if (mkqnximage_path is None):
        print("mkqnximage not found")
        print("Make sure to have installed QNX 8")
        print("And have run C:/Users/<username>/qnx800/qnxsdp-env.bat")
        exit(1)

    command = f"{mkqnximage_path} --build "
    if rpi:
        command += "--extra-dirs=+rasppi --type=rasppi --part-sizes=256:256 "
    else:
        command += "--type=vbox "

    execute(command, True)
    if rpi:
        print("RPi image built")
        print("The image is located at ") # Put path in here
        print("You can flash it onto a microSD card by using balena etcher")
    else:
        print("VM image built")
        print("You can start the VM by running \"mkqnximage --run\"")
        print("You can stop the VM by running \"mkqnximage --stop\"")
        print("You can access the VM by running \"mkqnximage --getip\" after starting it")
        print("You can then access the VM by running \"ssh -m hmac-sha2-256 root@<ip address from --getip>\" The password will be \"root\"")



if __name__ == "__main__":
    main()
