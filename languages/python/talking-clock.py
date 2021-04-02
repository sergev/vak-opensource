#!/usr/bin/env python3
#
# Talking clock, using my Google Home device as a media player.
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
    my_device = pychromecast.DeviceStatus(friendly_name = 'Abra',
                                          model_name = 'Cadabra',
                                          manufacturer = 'Unknown',
                                          uuid = uuid.UUID(my_uuid),
                                          cast_type = 'cast')
    my_cast_device = pychromecast.Chromecast(my_ip_addr,
                                             port = my_port,
                                             device = my_device)
    print("My Chromecast device:", my_cast_device)
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
    play_media(mc, "http://166.84.7.114/pub/media/talking-clock/service-login.oga", "application/ogg")

    # Speak the hour.
    filename = f"{hour:02d}_ru.mp3"
    play_media(mc, "http://166.84.7.114/pub/media/talking-clock/" + filename, "audio/mpeg")
