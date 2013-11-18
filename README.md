pebble-wallpapers
=================
Pebble watch-face wallpaper shuffler

Steps to changing wallpapers:
1) Place wallpapers in resources/src/images. Wallpapers should be 144x144 and single channel / indexed / only black and white. No shades of grey.
2) Edit resources/src/resource_map.json with the new wallpaper paths.
3) Edit src/wallpapers.c with a corrected WALLPAPER_COUNT and WALLPAPER_IDS
4) Run './waf build' from the base directory
5) Deploy build/wallpapers.pbw to the pebble

WARNING:
Don't add too many wallpapers to the watchface. Pebble has a size limit on pbw files. In my experience, if you add too many, the deploy step will fail when running through Android (no damage to the watch?). The installation bar gets half full then stops. Add wallpapers at your own risk.
