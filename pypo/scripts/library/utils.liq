
# Turn a source into an infaillible source.
# by adding blank when the source is not available.
# @param s the source to turn infaillible
# @category Source / Track Processing
def mksafe(s)
  fallback(id="mksafe",track_sensitive=false,[s,blank(id="safe_blank")])
end

# Alias for the <code>l[k]</code> notation.
# @category List
# @param a Key to look for
# @param l List of pairs (key,value)
def list.assoc(a,l)
  l[a]
end

# list.mem_assoc(key,l) returns true if l contains a pair
# (key,value)
# @category List
# @param a Key to look for
# @param l List of pairs (key,value)
def list.mem_assoc(a,l)
  v = list.assoc(a,l)
  # We check for existence, since "" may indicate
  # either a binding (a,"") or no binding..
  list.mem((a,v),l)
end

# Remove a pair from an associative list
# @category List
# @param a Key of pair to be removed
# @param l List of pairs (key,value)
def list.remove_assoc(a,l)
  list.remove((a,list.assoc(a,l)),l)
end

# Rewrite metadata on the fly using a list of (target,rules).
# @category Source / Track Processing
# @param l \
#    List of (target,value) rewriting rules.
# @param ~insert_missing \
#    Treat track beginnings without metadata as having empty ones. \
#    The operational order is: \
#    create empty if needed, map and strip if enabled.
# @param ~update \
#    Only update metadata. \
#    If false, only returned values will be set as metadata.
# @param ~strip \
#    Completly remove empty metadata. \
#    Operates on both empty values and empty metadata chunk.
def rewrite_metadata(l,~insert_missing=true,
                       ~update=true,~strip=false,
                       s)
  # We don't need to return all values, since
  # map_metadata only update returned values.
  # So, we simply apply all rewrite rules !
  def map(m)
    def apply(x)
      label = fst(x)
      value = snd(x)
      (label,value % m)
    end
    list.map(apply,l)
  end
  map_metadata(map,insert_missing=insert_missing,
               update=update,strip=strip,s)
end

# Add a skip function to a source
# when it does not have one
# by default
# @category Interaction
# @param s The source to attach the command to.
def add_skip_command(s) =
 # A command to skip
 def skip(_) =
   source.skip(s)
   "Done!"
 end
 # Register the command:
 server.register(namespace="#{source.id(s)}",
                 usage="skip",
                 description="Skip the current song.",
                 "skip",skip)
end

# Removes all metadata coming from a source
# @category Source / Track Processing
def drop_metadata(s)
  map_metadata(fun(_)->[],update=false,strip=true,insert_missing=false,s)
end

# Merge all tracks from a source, provided that it does not fail
# @category Source / Track Processing
def merge_tracks(s)
  sequence(merge=true,[s])
end

output.prefered=output.dummy
%ifdef output.oss
  output.prefered=output.oss
%endif
%ifdef output.alsa
  output.prefered=output.alsa
%endif
%ifdef output.pulseaudio
  output.prefered=output.pulseaudio
%endif
%ifdef output.ao
  output.prefered=output.ao
%endif
# Output to local audio card using the first available driver in this list:
#   ao, pulseaudio, alsa, oss, dummy
# @category Source / Output
def output.prefered(~id="",s)
  output.prefered(id=id,s)
end

in = fun () -> blank()
%ifdef input.oss
  in = fun () -> input.oss(id="oss_mic")
%endif
%ifdef input.alsa
  in = fun () -> input.alsa(id="alsa_mic")
%endif
%ifdef input.portaudio
  in = fun () -> input.portaudio(id="pa_mic")
%endif
# Create a source from the first available input driver in this list:
#   portaudio, alsa, oss, blank.
# @category Source / Input
def in()
  in()
end

# Output a stream using the 'output.prefered' operator. The input source does
# not need to be infallible, blank will just be played during failures.
# @param s the source to output
# @category Source / Output
def out(s)
  output.prefered(mksafe(s))
end

# Special track insensitive fallback that
# always skip current song before switching.
# @category Source / Track Processing
# @param ~input The input source
# @param f The fallback source
def fallback.skip(~input,f)
  def transition(a,b) =
    source.skip(a)
    # This eats the last remaining frame from a
    sequence([a,b])
  end
  fallback(track_sensitive=false,transitions=[transition,transition],[input,f])
end

# Compress and normalize, producing a more uniform and "full" sound.
# @category Source / Sound Processing
# @param s The input source.
def nrj(s)
  compress(threshold=-15.,ratio=3.,gain=3.,normalize(s))
end

# Multiband-compression.
# @category Source / Sound Processing
# @param s The input source.
def sky(s)
  # 3-band crossover
  low = filter.iir.eq.low(frequency = 168.)
  mh = filter.iir.eq.high(frequency = 100.)
  mid = filter.iir.eq.low(frequency = 1800.)
  high = filter.iir.eq.high(frequency = 1366.)

  # Add back
  add(normalize = false,
      [ compress(attack = 100., release = 200., threshold = -20.,
                 ratio = 6., gain = 6.7, knee = 0.3,
                 low(s)),
        compress(attack = 100., release = 200., threshold = -20.,
                 ratio = 6., gain = 6.7, knee = 0.3,
                 mid(mh(s))),
        compress(attack = 100., release = 200., threshold = -20.,
                 ratio = 6., gain = 6.7, knee = 0.3,
                 high(s))
      ])
end

# Simple crossfade.
# @category Source / Track Processing
# @param ~start_next Duration in seconds of the crossed end of track.
# @param ~fade_in Duration of the fade in for next track
# @param ~fade_out Duration of the fade out for previous track
# @param s The source to use
def crossfade(~id="",~start_next,~fade_in,~fade_out,s)
  s = fade.in(duration=fade_in,s)
  s = fade.out(duration=fade_out,s)
  fader = fun (a,b) -> add(normalize=false,[b,a])
  cross(id=id,conservative=true,duration=start_next,fader,s)
end

# Append speech-synthesized tracks reading the metadata.
# @category Source / Track Processing
# @param ~pattern Pattern to use
# @param s The source to use
def say_metadata
  p = 'say:$(if $(artist),"It was $(artist)$(if $(title),\", $(title)\").")'
  fun (s,~pattern=p) ->
    append(s,fun (m) -> request.queue(queue=[request.create(pattern % m)],
                                      interactive=false))
end

%ifdef soundtouch
# Increases the pitch, making voices sound like on helium.
# @category Source / Sound Processing
# @param s The input source.
def helium(s)
  soundtouch(pitch=1.5,s)
end
%endif

# Return true if process exited with 0 code.
# Command should return quickly.
# @category System
# @param command    Command to test
def test_process(command)
  lines =
    get_process_lines("(" ^ command ^ " >/dev/null 2>&1 && echo 0) || echo 1")
  if list.length(lines) == 0 then
    false
  else
    "0" == list.hd(lines)
  end
end

# Split an url of the form foo?arg=bar&arg2=bar2
# into ("foo",[("arg","bar"),("arg2","bar2")]
# @category String
# @param uri Url to split
def url.split(uri) = 
  ret = string.extract(pattern="([^\?]*)\?(.*)",uri)
  args = ret["2"]
  if args != "" then
    l = string.split(separator="&",args)
    def f(x) =
      ret = string.split(separator="=",x)
      (url.decode(list.nth(ret,0)),
       url.decode(list.nth(ret,1)))
    end
    l = list.map(f,l)
    (ret["1"],l)
  else
    (uri,[])
  end
end

# Register a server/telnet command to 
# update a source's metadata. Returns 
# a new source, which will receive the 
# updated metadata. Semantics is the 
# same as pre 1.0 insert_metadata operator, 
# i.e. @insert key1="val1",key2="val2",..@
# @category Source / Track Processing
# @param ~id Force the value of the source ID.
def server.insert_metadata(~id="",s) = 
  x = insert_metadata(id=id,s)
  insert = fst(x)
  s = snd(x)
  def insert(s) = 
    l = string.split(separator='([^=]+\s*=\s*"(\\"|[^"])*")\s*,\s*',s)
    def f(l,x) = 
      sub = fun (s) -> string.replace(pattern='\\"',fun (_) -> '"',s)
      if x != "" then
        ret = string.extract(pattern='([^=]+)\s*=\s*"((?:\\"|[^"])*)"',x)
        if ret["1"] != "" then
          list.append(l,[(ret["1"],
                          sub(ret["2"]))])
        else
          l
        end
      else
        l
      end
    end
    meta = list.fold(f,[],l)
    if meta != [] then
      insert(meta)
      "Done"
    else
      "Syntax error or no metadata given. \
       Use key1=\"val1\",key2=\"val2\",.."
    end
  end
  id = source.id(s)
  server.register(namespace="#{id}",
                  description="Insert a metadata chunk.",
                  usage="insert key1=\"val1\",key2=\"val2\",..",
                  "insert",insert)
  s
end

# Get the base name of a path.
# Implemented using the corresponding shell command.
# @category System
# @param s Path
def basename(s)
  lines = get_process_lines("basename #{quote(s)}")
  if list.length(lines) > 0 then
    list.hd(lines)
  else
    # Don't know what to do.. output s
    s
  end
end

# Get the directory name of a path.
# Implemented using the corresponding shell command.
# @category System
# @param s Path
# @param ~default Value returned in case of error.
def dirname(~default="/nonexistent",s)
  lines = get_process_lines("dirname #{quote(s)}")
  if list.length(lines) > 0 then
    list.hd(lines)
  else
    default
  end
end

# Read some value from standard input (console).
# @category System
# @param ~hide Hide typed characters (for passwords).
def read(~hide=false)
  if hide then
    system("stty -echo")
  end
  s = list.hd(get_process_lines("read BLA && echo $BLA"))
  if hide then
    system("stty echo")
  end
  print("")
  s
end

# Generic mime test. First try to use file.mime if it exist.
# Otherwise try to get the value using the file binary.
# Returns "" (empty string) if no value can be find.
# @category System
# @param file The file to test
def get_mime(file) =
  def file_method(file) =
    if test_process("which file") then
      list.hd(get_process_lines("file -b --mime-type \
                                  #{quote(file)}"))
    else
      ""
    end
  end
  def mime_method(file) =
    ret = ""
    %ifdef file.mime
      ret = file.mime(file)
    %endif
    ret
  end
  # First try mime method
  ret = mime_method(file)
  if ret != "" then
    ret 
  else
    # Now try file method
    file_method(file)
  end
end


# Remove low frequencies often produced by microphones.
# @category Source / Sound Processing
# @param s The input source.
def mic_filter(s)
  filter(freq=200.,q=1.,mode="high",s)
end

# Creates a source that fails to produce anything.
# @category Source / Input
def fail()
  fallback([])
end

# Creates a source that plays only one track of the input source.
# @category Source / Track Processing
# @param s The input source.
def once(s)
  sequence([s,fail()])
end

# Crossfade between tracks, taking the respective volume levels into account in
# the choice of the transition.
# @category Source / Track Processing
# @param ~start_next   Crossing duration, if any.
# @param ~fade_in      Fade-in duration, if any.
# @param ~fade_out     Fade-out duration, if any.
# @param ~width        Width of the volume analysis window.
# @param ~conservative Always prepare for a premature end-of-track.
# @param ~default      Transition used when no rule applies \
#                      (default: sequence).
# @param ~high         Value, in dB, for loud sound level
# @param ~medium       Value, in dB, for medium sound level
# @param ~margin       Margin to detect sources that have too different \
#                      sound level for crossing.
# @param s             The input source.
def smart_crossfade (~start_next=5.,~fade_in=3.,~fade_out=3.,
                     ~default=(fun (a,b) -> sequence([a, b])),
                     ~high=-15., ~medium=-32., ~margin=4.,
                     ~width=2.,~conservative=false,s)
  fade.out = fade.out(type="sin",duration=fade_out)
  fade.in  = fade.in(type="sin",duration=fade_in)
  add = fun (a,b) -> add(normalize=false,[b, a])
  log = log(label="smart_crossfade")

  def transition(a,b,ma,mb,sa,sb)

    list.iter(fun(x)-> log(level=4,"Before: #{x}"),ma)
    list.iter(fun(x)-> log(level=4,"After : #{x}"),mb)

    if
      # If A and B are not too loud and close, fully cross-fade them.
      a <= medium and b <= medium and abs(a - b) <= margin
    then
      log("Old <= medium, new <= medium and |old-new| <= margin.")
      log("Old and new source are not too loud and close.")
      log("Transition: crossed, fade-in, fade-out.")
      add(fade.out(sa),fade.in(sb))

    elsif
      # If B is significantly louder than A, only fade-out A.
      # We don't want to fade almost silent things, ask for >medium.
      b >= a + margin and a >= medium and b <= high
    then
      log("new >= old + margin, old >= medium and new <= high.")
      log("New source is significantly louder than old one.")
      log("Transition: crossed, fade-out.")
      add(fade.out(sa),sb)

    elsif
      # Opposite as the previous one.
      a >= b + margin and b >= medium and a <= high
    then
      log("old >= new + margin, new >= medium and old <= high")
      log("Old source is significantly louder than new one.")
      log("Transition: crossed, fade-in.")
      add(sa,fade.in(sb))

    elsif
      # Do not fade if it's already very low.
      b >= a + margin and a <= medium and b <= high
    then
      log("new >= old + margin, old <= medium and new <= high.")
      log("Do not fade if it's already very low.")
      log("Transition: crossed, no fade.")
      add(sa,sb)

    # What to do with a loud end and a quiet beginning ?
    # A good idea is to use a jingle to separate the two tracks,
    # but that's another story.

    else
      # Otherwise, A and B are just too loud to overlap nicely,
      # or the difference between them is too large and overlapping would
      # completely mask one of them.
      log("No transition: using default.")
      default(sa, sb)
    end
  end

  smart_cross(width=width, duration=start_next, conservative=conservative,
              transition,s)
end

# Custom playlist source written using the script language.
# Will read directory or playlist, play all files and stop
# @category Source / Input
# @param ~random Randomize playlist content
# @param ~on_done Function to execute when the playlist is finished
# @param uri Playlist URI
def playlist.once(~random=false,~on_done={()},uri)
  x = ref 0
  def playlist.custom(files)
    length = list.length(files)
    if length == 0 then
      log("Empty playlist..")
      fail ()
    else
      files =
        if random then
          list.sort(fun (x,y) -> int_of_float(random.float()), files)
        else
          files
        end
      def next () =
        state = !x
        file =
        if state < length then
          x := state + 1
          list.nth(files,state)
        else
          # Playlist finished
          on_done ()
          ""
        end
        request.create(file)
      end
      request.dynamic(next)
    end
  end
  if test_process("test -d #{quote(uri)}") then
    files = get_process_lines("find #{quote(uri)} -type f | sort")
    playlist.custom(files)
  else
    playlist = request.create.raw(uri)
    result =
      if request.resolve(playlist) then
        playlist = request.filename(playlist)
        files = playlist.parse(playlist)
        files = list.map(snd,files)
        playlist.custom(files)
      else
        log("Couldn't read playlist: request resolution failed.")
        fail ()
      end
    request.destroy(playlist)
    result
  end
end

# Mixes two streams, with faded transitions between the state when only the
# normal stream is available and when the special stream gets added on top of
# it.
# @category Source / Track Processing
# @param ~delay   Delay before starting the special source.
# @param ~p       Portion of amplitude of the normal source in the mix.
# @param ~normal  The normal source, which could be called the carrier too.
# @param ~special The special source.
def smooth_add(~delay=0.5,~p=0.2,~normal,~special)
  d = delay
  fade.final = fade.final(duration=d*2.)
  fade.initial = fade.initial(duration=d*2.)
  q = 1. - p
  c = amplify
  fallback(track_sensitive=false,
           [special,normal],
           transitions=[
             fun(normal,special)->
               add(normalize=false,
                   [c(p,normal),
                    c(q,fade.final(type="sin",normal)),
                    sequence([blank(duration=d),c(q,special)])]),
             fun(special,normal)->
               add(normalize=false,
                   [c(p,normal),
                    c(q,fade.initial(type="sin",normal))])
           ])
end

# Restrict a source to play only when a predicate is true.
# @category Source / Track Processing
# @param pred The predicate, typically a time interval such as \
#             <code>{10h-10h30}</code>.
def at(pred,s)
  switch([(pred,s)])
end

# Execute a given action when a predicate is true.
# This will be run in background.
# @category System
# @param ~freq Frequency for checking the predicate, in seconds.
# @param ~pred Predicate indicating when to execute the function, \
#              typically a time interval such as <code>{10h-10h30}</code>.
# @param f Function to execute when the predicate is true.
def exec_at(~freq=1.,~pred,f)
  def check()
    if pred() then
      f()
    end
    freq
  end
  add_timeout(freq,check)
end

# Register the replaygain protocol
def replaygain_protocol(arg,delay)
 # The extraction program
 extract_replaygain = "#{configure.libdir}/extract-replaygain"
 x = get_process_lines("#{extract_replaygain} #{quote(arg)}")
 if list.hd(x) != "" then
  ["annotate:replay_gain=\"#{list.hd(x)}\":#{arg}"]
 else
  [arg]
 end
end
add_protocol("replay_gain", replaygain_protocol)

# Enable replay gain metadata resolver. This resolver will
# process any file decoded by liquidsoap and add a @replay_gain@
# metadata when this value could be computed. For a finer-grained
# replay gain processing, use the @replay_gain@ protocol.
# @category Liquidsoap
# @param ~extract_replaygain The extraction program
def enable_replaygain_metadata(
       ~extract_replaygain="#{configure.libdir}/extract-replaygain")
  def replaygain_metadata(file)
    x = get_process_lines("#{extract_replaygain} \
                              #{quote(file)}")
    if list.hd(x) != "" then
      [("replay_gain",list.hd(x))]
    else
      []
    end
  end
  add_metadata_resolver("replay_gain", replaygain_metadata)
end

# Create a log of clock times for all the clocks initially present.
# The log is in a simple format which you can directly use with gnuplot.
# @category Liquidsoap
# @param ~interval Polling interval.
# @param ~delay    Delay before setting up the clock logger. This should \
#                  be used to ensure that the logger starts only after \
#                  the clocks are created.
# @param unlabeled Path of the log file.
def log_clocks(~delay=0.,~interval=1.,logfile)
  # Get the current clocks
  clocks = list.map(fst,get_clock_status())
  # Column headers
  system("echo \# #{string.concat(separator=' ',clocks)} > #{(logfile:string)}")
  def report()
    status = get_clock_status()
    status = list.map(fun (x) -> (fst(x),string_of(snd(x))), status)
    status = list.map(fun (c) -> status[c], clocks)
    system("echo #{string.concat(separator=' ',status)} >> #{logfile}")
    interval
  end
  if delay<=0. then
    add_timeout(interval,report)
  else
    add_timeout(delay,{add_timeout(interval,report) (-1.)})
  end
end
