###########################################
# liquidsoap config file                  #
###########################################
# This config assumes that there are 
# two instances of LS running
# the "scheduler" & the "fallback" instance


###########################################
# general settings                        #
###########################################

log_file = "/var/log/pypo/<script>.log"
log_level = 3

# archive directory
archive_dir = "/opt/pypo/archive/"

# list pointing to the current couchcaster mountpoint 
couchcaster_list = "http://vdeb.openbroadcast.ch/mod/ml/api/pypo/current_couchcaster"


###########################################
# stream settings                         #
###########################################
icecast_host = "127.0.0.1"
icecast_port = 8000
icecast_pass = "hackme"

# mountpoints
mount_scheduler = "airtime.mp3"

# mount intra is used for scheduler >>> fallback stream
mount_intra = "pypo_intra"

# intra-LS streaming (no icecast here)
intra_host = "127.0.0.1"
intra_port = 9000
intra_pass = "hackme"

