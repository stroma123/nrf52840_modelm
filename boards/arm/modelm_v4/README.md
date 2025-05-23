# modelm

Driver board for the IBM Model M mechanical keyboard, based on the nrf52840.

## Building ZMK firmware

### Locally using Docker image

    docker run --interactive --tty --name zmk-modelm --workdir /zmk \
        --volume "./config:/zmk-config" \
        --volume "./zmk:/zmk" \
        --volume ".:/boards" \
        --user="$(id -u):$(id -g)" \
        zmkfirmware/zmk-dev-arm:3.5 \
        west build /zmk/app --pristine --board "modelm_v4" \
        -- -DSHIELD=settings_reset -DZMK_CONFIG="/zmk-config" \
        -DZMK_EXTRA_MODULES="/boards" 

### Locally using native installation

```
west build --board modelm_v4 app
```
