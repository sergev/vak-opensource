#!/usr/bin/env python3
#
# Talking clock, using my Google Home device as a media player.
#
# Prerequisites:
#       pip3 install --user pychromecast
#
# Use 'crontab -e' to run this script every hour, like:
#       0 * * * * $HOME/bin/talking-clock.py
#
import pychromecast, uuid, time, datetime

#
# Get list of all Chromecast devices.
# The local computer must be in the same Wi-Fi network.
#
def print_all_chromecasts():
    all_cast_devices = pychromecast.get_chromecasts()
    print("All Chromecast devices:", all_cast_devices)

#
# Find media controller by unique ID.
#
def find_media_controller(my_uuid):
    all_cast_devices = pychromecast.get_chromecasts()
    my_cast_device = next(x for x in all_cast_devices if str(x.device.uuid) == my_uuid)
    return my_cast_device.media_controller

#
# Open media controller with given IP address, port and unique ID.
#
def open_media_controller(my_uuid, my_ip_addr, my_port):
    host = (my_ip_addr, my_port, uuid.UUID(my_uuid), 'Cadabra', 'Abra')
    my_cast_device = pychromecast.get_chromecast_from_host(host)
    print("My Chromecast device:", my_cast_device)
    my_cast_device.wait()
    return my_cast_device.media_controller

#
# Play a video on my Chromecast device.
#
def play_media(mc, url, media_type):
    print("Playing media:", url)
    mc.play_media(url, content_type = media_type)
    mc.block_until_active()
    mc.play()

    # Wait until done.
    while True:
        time.sleep(1)
        mc.update_status()
        if mc.status.player_is_idle:
            break

#
# Get time.
#
now = datetime.datetime.today()
hour = now.hour
minute = now.minute
print(f"Time: {hour}:{minute}")

#
# When the time is exact, voice it.
#
if minute == 0:
    # Open my media controller.
    mc = open_media_controller("c854c4ea-517a-29cf-e083-1e63659ea1c6", "192.168.86.75", 8009)

    # Play a chime, to get attention.
    play_media(mc, "https://pub.sergev.org/media/talking-clock/service-login.oga", "application/ogg")

    # Speak the hour.
    filename = f"{hour:02d}_ru.mp3"
    play_media(mc, "https://pub.sergev.org/media/talking-clock/" + filename, "audio/mpeg")
