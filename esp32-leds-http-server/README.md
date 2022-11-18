# Leds

## Spawn rocket

```bash
# resolving of IP takes 5 seconds, so it is better to cache it
IP=$(dscacheutil -q host -a name esp32leds.local | grep ip | cut -d' ' -f2)

BODY=$(j .speed: 10, .trail: 10, hue: 0, sat: 255, val: 255)

# this takes a lot of time because of the long resolve of IP
# time curl -v -H 'Content-type: application/json' -d $BODY http://esp32leds.local/spawn

time curl -v -H 'Content-type: application/json' -d $BODY http://$IP/spawn
```

```javascript
// js fetch version
fetch('http://esp32leds.local/spawn', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/json'
  },
  body: JSON.stringify({
    speed: 10,
    trail: 10,
    hue: 0,
    sat: 255,
    val: 255
  })
})
```
