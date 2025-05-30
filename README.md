# modelm_keyboard
Project for an USB/BLE IBM Model M keyboard controller

This project is an attempt to replace the original controller PCB of the IBM Model M keyboard with a modern solution.
The firmware for the project relies on Nordic nRF52840 microcontroller and is based on the [ZMK](https://github.com/zmkfirmware) project.
It allows to use the keyboard via USB connector, or via Bluetooth.

## Tools

You need a SWD programmer (STLink) with 4 pins for the initial flash programming of the board.

## Setup

* Use your favorite tool to flash the bootloader to the nrf52840
* Use drag-and-drop of the uf2 build output to flash the keyboard firmware

## Build the Firmware

There are several options to build the firmware.

### Using GitHub action

`push` and `pull_request` trigger a github-action to build the firmware. To locate the build output
go to the `Actions` tab and open the respective workflow run. There you find a link to the artifacts
generated by the run. You can download it and unpack the `.zip` archive to access the `.UF2` file.

### Locally using Docker image

This option describes Docker container method to build the firmware, only native Docker CLI used.
It assumes you already have Docker installed and your user has permission to run `docker`. If you need to run `docker` as the **root** user, please remember to prepend `sudo` to the docker commands below.

This is **not** Docker Desktop/VS Code approach described in ZMK [Getting Started](https://zmk.dev/docs/development/local-toolchain/setup/docker) setup.

* Clone the [ZMK](https://github.com/zmkfirmware) project into this repo.

        git clone https://github.com/zmkfirmware/zmk

* Now initialize and update the ZMK. Note this step is needed only once and may take some time until it is completed.

        docker run --rm --interactive --tty --name zmk-modelm --workdir /zmk \
            --volume "./config:/zmk-config" \
            --volume "./zmk:/zmk" \
            --volume ".:/boards" \
            --user="$(id -u):$(id -g)" \
            zmkfirmware/zmk-dev-arm:3.5 \
            sh -c 'west init -l /zmk/app/; west update'

  Note, if you encounter an error such as: `docker: Error response from daemon: create ./config: "./config" includes invalid characters for a local volume name, only "[a-zA-Z0-9][a-zA-Z0-9_.-]" are allowed. If you intended to   pass a host directory, use absolute path.` you may need to update `docker` on your system.

* Build the firmware:

        docker run --interactive --tty --name zmk-modelm --workdir /zmk \
            --volume "./config:/zmk-config" \
            --volume "./zmk:/zmk" \
            --volume ".:/boards" \
            --user="$(id -u):$(id -g)" \
            zmkfirmware/zmk-dev-arm:3.5 \
            west build /zmk/app --pristine --board "modelm" \
            -- -DZMK_CONFIG="/zmk-config" \
            -DZMK_EXTRA_MODULES="/boards" 

* Copy the firmware file and remove the container:

        docker cp zmk-modelm:/zmk/build/zephyr/zmk.uf2 ./modelm.uf2
        docker container rm zmk-modelm

For convenience, while developing/testing, you may leave the container running.

* Create the container and just run `bash`, note the `--detach` option.

        docker run --detach --interactive --tty --name zmk-modelm --workdir /zmk \
            --volume "./config:/zmk-config" \
            --volume "./zmk:/zmk" \
            --volume ".:/boards" \
            --user="$(id -u):$(id -g)" \
            zmkfirmware/zmk-dev-arm:3.5 /bin/bash

* Now the container is detached but is still running, build the firmware with `docker exec` command.

        docker exec --interactive --tty zmk-modelm \
            west build /zmk/app --pristine --board "modelm" \
            -- -DZMK_CONFIG="/zmk-config" \
            -DZMK_EXTRA_MODULES="/boards"
        
        docker cp zmk-modelm:/zmk/build/zephyr/zmk.uf2 ./modelm.uf2

* To remove the container, you need to stop it first.

        docker stop zmk-modelm
        docker container rm zmk-modelm

### Locally using native installation

Set the ZMK and Zephyre projects up as described in [Toolchain Setup](https://zmk.dev/docs/development/setup).
Go to the ZMK base directory, then run

    cd app
    west build --board modelm . -- -DZMK_EXTRA_MODULES=(..)/modelm/

Replace `(..)` with the path of this project. Note, that the tilde (`~`) is not evaluated correctly in that pathname.

## Bootloader

The project uses a modified [Adafruit](https://github.com/stroma123/Adafruit_nRF52_Bootloader.git) bootloader.
