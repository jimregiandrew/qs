# Vision

I want a standard file format for quantity sequence data that is understood by
browsers, apps etc. Just like JPEG for photographic images, MPEG variants for
videos, and mp3 etc for music.

# What is quantity sequence data ?

This is the best name I could come up with for data that is a function of time,
or more generally is a sequence. Also known as time-series or digital data. The
following are all a function of time, and are for some time period:

1. latitude, longitude, speed, distance travelled, fuel used, rpm, and
(3-dimensional) acceleration values every second for a vehicle.
2. latitude, longitude and temperature reading for a refreigerated cargo, every
30 seconds.
3. lateral, medial and vertical acceleration values every millisecond 
encompassing a period with an (vehicular) impact.
4. NOX ppm every minute.
5. engine parameters for a vehicle, where each parameter has a different update
interval.
6. Mood scores for mood diaries.
7. Historical stock market prices
8. Data logger data

This is the file format for "the internet of things" for sensors. Smart phones 
could be logging a lot of datax, that would be so useful if we could understand
(know the format) and share it. (Think acceleration, speed and pollution values
of my bus trip, or day).

Wikipedia has a definition of quantity. This format is for any numeric or 
enumerated data.

# Don't we have a file format for this already?

I can't find any that satisfy the requirements. The closest is HDF. From what I can 
see it is targeted at scientific data where precision and volume is important. It
values precision over compressed size. I want to be able to trade them off. What 
I want is to be able to store chunks of data that I can archive and share with 
anyone - just like I can share a JPEG image with anyone. Longer term it would
be great to have this format understood by browsers. e.g. Store data
in a DB, in a compact form, access these from a Browser, and have the Browser
decode the data for me. Just like is done with JPEG images.

#What stage is the code here at?

I am attempting a proof of concept. That is to write source code that quantizes
(quantity sequence) data, and encodes/decodes it using a static (or optimized)
Huffman code. If you know about compression then you will know this is 
something relatively basic. I haven't got this far yet and even that is just 
a start.

From my experience a lot of quantities can be compressed to the order of 1 bit
per second - e.g. latitude, longitude, speed etc. This is 3.6k bytes for an 8
hour period (e.g. working day) per quantity, and 72K for 20 quantities. That is 
not a lot of data to save for a record of a day. 

To do a proper standard for compression will take work from a lot of people or
a few experts. I am not one.

# File format

Initial idea (_ is a delimiter of diferent symbols):

Nq_Nn_T0_QS0_QS1..QSN-1

Nq : number of quantities
Nn : length of sequence (number of samples of each quantity)
T0 : start (Unix) epoch seconds
QS0,1,..N-1 : qantity sequence 0, prefixed with quantization step size.

Note: QSn can include quantity specifier, or have bitmask following e.g. Nq to
indicate which of std, quantities are included. Have num. of non-std ones, 
plus their definition following.

# Goal

To have a file format that:

1. is self describing.
2. is compact (A random goal: within 2:1 of the best known compression method)
3. handles one-dimensional and multi-dimensional functions of time (or 
sequences).
4. can be generated and decoded on an embedded device.
5. can be configured with different levels of loss (i.e. trade compression 
for file size).

By self describing we mean it defines the data contained therein: name, unit,
precision (quantization). As well as giving the actual data. The JFIF for JPEG
is an example of a self-describing file in this regard. Self describing allows
an application to decode a tnz file and to visualize the data (e.g. graph it).

We want a format that allows a human to be able to inspect the (decoded) data 
(e.g. in a graphing tool) and understand it. We want a format that, as much as
possible,allow programs to understand the data.

The idea is that sensors can send "snippets" of data that anyone can decode and
understand, and these snippets can be stored and served from DB's (natively).

What I want is

1. A standard format I can load and decode in a browser just like JPEG (EXIF
or JFIF) can be: e.g. via an AJAX call
2. A compact format so I don't have to send lots of data (think mobile or 
any bandlimited network).
3. A format that is not encumbered with intellectual property (patents)
4. A relatively simple format that can be encoded/decoded on an embedded 
device. More complicated encodings are fine, as long as not necessary and 
the decoding is relatively simple.

It really just a file interchange format - defining the meaning of the data contained therein.

The applications are endless.

