# model_x

Driver board for the IBM Model M mechanical keyboard, based on the SuperMini nrf52840 module and 74HC595 shift registers.
Heavily inspired from [NilsFC/Model-X](https://github.com/NilsFC/Model-X)

## Building ZMK firmware

### Locally using Docker image

    docker run --rm --interactive --tty --name zmk-modelm --workdir /zmk \
        --volume "./config:/zmk-config" \
        --volume "./zmk:/zmk" \
        --volume ".:/boards" \
        --user="$(id -u):$(id -g)" \
        zmkfirmware/zmk-dev-arm:3.5 \
        west build /zmk/app -p -b nice_nano_v2 -- -DSHIELD=model_x -DZMK_CONFIG="/zmk-config" -DZMK_EXTRA_MODULES="/boards"

### Locally using native installation

```
west build --board nrf52840_modelm -- -DSHIELD=model_x app
```

