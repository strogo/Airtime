#!/usr/bin/env python
# -*- coding: utf-8 -*-

# author Jonas Ohrstrom <jonas@digris.ch>

"""
Python part of radio playout (pypo)

This function acts as a gateway between liquidsoap and the obp-api.
Mainliy used to tell the plattform what pypo/LS does.

Main case: 
 - whenever LS starts playing a new track, its on_metadata callback calls
   a function in ls (notify(m)) which then calls the pythin script here
   with the currently starting filename as parameter 
 - this python script takes this parameter, tries to extract the actual
   media id from it, and then calls back to obp via api to tell about


"""

# python defaults (debian default)
import time
import os
import traceback
from optparse import *
import sys
import time
import datetime
import logging
import logging.config
import urllib
import urllib2
import string

import socket

# additional modules (should be checked)
from configobj import ConfigObj

# custom imports
from util import *
from obp import *



PYPO_VERSION = '0.9'
OBP_MIN_VERSION = 2010040501 # required obp version


#set up command-line options
parser = OptionParser()

# help screeen / info
usage = "%prog [options]" + " - notification gateway"
parser = OptionParser(usage=usage)

#options
parser.add_option("-p", "--playing", help="Tell daddy what is playing right now", metavar="path")

# parse options
(options, args) = parser.parse_args()

# configure logging
logging.config.fileConfig("logging.cfg")

# loading config file
try:
    config = ConfigObj('config.cfg')
    TMP_DIR = config['tmp_dir']
    BASE_URL = config['base_url']
    OBP_API_BASE = BASE_URL + 'mod/medialibrary/'
    EXPORT_SOURCE = config['export_source']
    
    OBP_STATUS_URL = OBP_API_BASE + 'status/version/json'
    OBP_API_KEY = config['obp_api_key']
    
except Exception, e:
    print 'error: ', e
    sys.exit()
    
    
class Global:
    def __init__(self):
        print
        
    def selfcheck(self):
        
        self.api_auth = urllib.urlencode({'api_key': OBP_API_KEY})
        self.api_client = ApiClient(OBP_API_BASE, self.api_auth)
        
        obp_version = self.api_client.get_obp_version()
        
        if obp_version == 0:
            print '#################################################'
            print 'Unable to get OBP version. Is OBP up and running?'
            print '#################################################'
            print
            sys.exit()
         
        elif obp_version < OBP_MIN_VERSION:
            print 'OBP version: ' + str(obp_version)
            print 'OBP min-version: ' + str(OBP_MIN_VERSION)
            print 'pypo not compatible with this version of OBP'
            print
            sys.exit()
         
        else:
            print 'OBP API: ' + str(OBP_API_BASE)
            print 'OBP version: ' + str(obp_version)
            print 'OBP min-version: ' + str(OBP_MIN_VERSION)
            print 'pypo is compatible with this version of OBP'
            print

class Notify:
    def __init__(self):

        self.tmp_dir = TMP_DIR 
        self.export_source = EXPORT_SOURCE
        
        self.api_auth = urllib.urlencode({'api_key': OBP_API_KEY})
        self.api_client = ApiClient(OBP_API_BASE, self.api_auth)

    
    def start_playing(self, options):
        logger = logging.getLogger("start_playing")

        tnow = time.localtime(time.time())


        path = options
        
        try:
            file = path.split("/")[-1:][0]
            if file.find('_cue_') > 0:
                id = file.split("_cue_")[0]
            else:
                id = file.split(".")[-2:][0]
            
        except Exception, e:
            #print e
            id = False
            
        try:
            id = int(id)
        except Exception, e:
            #print e
            id = False
        
        print 
        print "Media ID: ", 
        print id            
          
        
        
        # self.api_client.update_start_playing(id, self.export_source, path) 
        
        txt = "test this update"
        
        
        
        # Echo client program


        HOST = '172.16.16.128'    # The remote host
        PORT = 50008              # The same port as used by the server
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT))
        
        s.send('client_zzzz')
        s.send("\r\n")
        data = s.recv(1024)
        print data;
        
        s.send('RS_DLS_VERSION' + ' ' + '1')
        s.send("\r\n")
        data = s.recv(1024)
        print data;
        
        s.send('SERVICE' + ' ' + 'OPENBRO+')
        s.send("\r\n")
        
        s.send('PASSWORD' + ' ' + 'OPENBRO+')
        s.send("\r\n")
        data = s.recv(1024)
        print data;
        
        s.send('CLEAR_DLS')
        s.send("\r\n")
        
        s.send('SET_DLS' + ' ' + txt)
        s.send("\r\n")
        data = s.recv(1024)
        print data;
        
        s.close()
        
        print data
        
        if data == "session":
            print 'KKK'
        
        
        time.sleep(0.1)
        
        
        
        
        print 'DONE'  

     
            

if __name__ == '__main__':
  
    print
    print '#########################################'
    print '#           *** pypo  ***               #'
    print '#     pypo notification gateway         #'
    print '#########################################'
    print
    
    # initialize
    g = Global()
    g.selfcheck()
    n = Notify()


run = True
while run == True:
    
    logger = logging.getLogger("pypo notify")
            
    while options.playing:
        try: n.start_playing(options.playing)
        except Exception, e:
            print e
        sys.exit()  
        

            

    sys.exit()
