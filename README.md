pebble-wallpapers
=================
Pebble watch-face wallpaper shuffler

Steps to changing wallpapers:<br/>
1) Place wallpapers in resources/images. Wallpapers should be 144x144 and single channel / indexed / only black and white. No shades of grey.<br/>
2) Edit appinfo.json with the new wallpaper paths.<br/>
3) Edit src/wallpapers.c with a corrected WALLPAPER_COUNT and WALLPAPER_IDS<br/>
4) Edit change_wallpaper_random in src/wallpapers.c for any color swapping information<br/>
5) Run 'pebble build' from the base directory<br/>
6) Run 'pebble install --phone <phone ip address>'. Ensure that the pebble phone app is enabled for developer connections.<br/>
<br/>
WARNING:<br/>
Don't add too many wallpapers to the watchface. Pebble has a size limit on pbw files. In my experience, if you add too many, the deploy step will fail when running through Android (no damage to the watch?). The installation bar gets half full then stops. Add wallpapers at your own risk.<br/>
