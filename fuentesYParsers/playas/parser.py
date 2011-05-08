#!/usr/bin/python
# -*- coding: utf-8 -*-
import csv
import sgmllib
from HTMLParser import HTMLParser
import os
import codecs
import cStringIO
import StringIO
import glob
import codecs
import copy
import math

def utmToLatLng(zone, easting, northing, northernHemisphere=True):
    if not northernHemisphere:
        northing = 10000000 - northing

    a = 6378137
    e = 0.081819191
    e1sq = 0.006739497
    k0 = 0.9996

    arc = northing / k0
    mu = arc / (a * (1 - math.pow(e, 2) / 4.0 - 3 * math.pow(e, 4) / 64.0 - 5 * math.pow(e, 6) / 256.0))
    ei = (1 - math.pow((1 - e * e), (1 / 2.0))) / (1 + math.pow((1 - e * e), (1 / 2.0)))
    ca = 3 * ei / 2 - 27 * math.pow(ei, 3) / 32.0
    cb = 21 * math.pow(ei, 2) / 16 - 55 * math.pow(ei, 4) / 32
    cc = 151 * math.pow(ei, 3) / 96
    cd = 1097 * math.pow(ei, 4) / 512
    phi1 = mu + ca * math.sin(2 * mu) + cb * math.sin(4 * mu) + cc * math.sin(6 * mu) + cd * math.sin(8 * mu)
    n0 = a / math.pow((1 - math.pow((e * math.sin(phi1)), 2)), (1 / 2.0))
    r0 = a * (1 - e * e) / math.pow((1 - math.pow((e * math.sin(phi1)), 2)), (3 / 2.0))
    fact1 = n0 * math.tan(phi1) / r0

    _a1 = 500000 - easting
    dd0 = _a1 / (n0 * k0)
    fact2 = dd0 * dd0 / 2

    t0 = math.pow(math.tan(phi1), 2)
    Q0 = e1sq * math.pow(math.cos(phi1), 2)
    fact3 = (5 + 3 * t0 + 10 * Q0 - 4 * Q0 * Q0 - 9 * e1sq) * math.pow(dd0, 4) / 24

    fact4 = (61 + 90 * t0 + 298 * Q0 + 45 * t0 * t0 - 252 * e1sq - 3 * Q0 * Q0) * math.pow(dd0, 6) / 720

    lof1 = _a1 / (n0 * k0)
    lof2 = (1 + 2 * t0 + Q0) * math.pow(dd0, 3) / 6.0
    lof3 = (5 - 2 * Q0 + 28 * t0 - 3 * math.pow(Q0, 2) + 8 * e1sq + 24 * math.pow(t0, 2)) * math.pow(dd0, 5) / 120
    _a2 = (lof1 - lof2 + lof3) / math.cos(phi1)
    _a3 = _a2 * 180 / math.pi

    latitude = 180 * (phi1 - fact1 * (fact2 + fact3 + fact4)) / math.pi

    if not northernHemisphere:
        latitude = -latitude

    longitude = ((zone > 0) and (6 * zone - 183.0) or 3.0) - _a3

    return (latitude, longitude)


class CloginParser(sgmllib.SGMLParser): #La que va entre parentesis es la clase madre. Estamos heredando. 
    def parse(self, s): 
        self.mydict=dict(	)
        self.feed(s)
        self.close()
        if ( len( self.mydict.keys()) >3 ):
        	[lon, lat]= utmToLatLng(float(self.mydict["huso"]),float(self.mydict["x"]),float(self.mydict["y"]))
        	self.mydict["lon"]=lon
        	self.mydict["lat"]=lat
        	del self.mydict["huso"]
        	del self.mydict["x"]
        	del self.mydict["y"]
		#(self.mydict["huso"],self.mydict["x"],self.mydict["y"])
        

        return self.mydict;
        
	 
 
 
    def __init__(self, verbose=0):
		sgmllib.SGMLParser.__init__(self, verbose) 
		self.mydict=dict()
		self.status={}
		self.inside=False
        
     # Este parser es parecido a un SAX de xml. La clase madre va llamando a metodos especificos para cada etiqueta que encuentra.
     #En nuestro caso nos interesa cojer del codigo html del formulario los parametros login y hash. 
    def start_td(self, aAttributes):
    	self.inside=True
        return
        
    def end_td(self):
    	self.inside=False;
    
            
    def handle_data(self,data):    	
    	if( data == "Provincia:"):
    		self.status="provincia"
    		return
    	elif (data == "Municipio:"):
    		self.status="municipio"
    		return
    	elif (data == "Comunidad Autónoma:"):
    		self.status="comunidad"
    		return    		
    	elif (data == "Zona Agua Baño:"):
    		self.status="nombre"
    		return   
    	
    	elif (data == "X:"):
    		self.status="x"
    		return   

    	elif (data == "Y:"):
    		self.status="y"
    		return 
    		
    	elif (data == "Huso:"):
    		self.status="huso"
    		return   
    		  		  		   		   			
    			
    	if(self.inside==True):
	    	if (self.status=="provincia" or self.status=="municipio"  or self.status=="comunidad" or self.status=="nombre" or self.status=="x" or self.status=="y" or self.status=="huso"):
	    		self.mydict[self.status]=data.decode("utf-8")
	    		self.status=""
    	return
    	
class UnicodeWriter(object):

    def __init__(self, f, dialect=csv.excel_tab, encoding="utf-8", **kwds):
        # Redirect output to a queue
        self.queue = StringIO.StringIO()
        self.writer = csv.writer(self.queue, dialect=dialect, **kwds)
        self.stream = f
        self.encoding = encoding
    
    def writerow(self, row):
        # Modified from original: now using unicode(s) to deal with e.g. ints
       # for s in row:
		 #	try:
		 	#self.writer.writerow([unicode(s).encode("utf-8") for s in row])
		 	#print row

		# 	except ValueError:
		#		a=0
				
        # Fetch utf-8 output from the queue ...
        self.writer.writerow([unicode(s).encode("utf-8") for s in row])
        data = self.queue.getvalue()
        data = data.decode("utf-8")
        # ... and reencode it into the target encoding
        data = data.encode(self.encoding)
        # write to the target stream
        self.stream.write(data)
        # empty queue
        self.queue.truncate(0)
    
    def writerows(self, rows):
        for row in rows:
            self.writerow(row)

            
class UnicodeDictWriter(UnicodeWriter):    
    def __init__(self, f, fields, dialect=csv.excel_tab,
            encoding="utf-8", **kwds):
        super(UnicodeDictWriter, self).__init__(f, dialect, encoding, **kwds)
        self.fields = fields
    
    def writerow(self, drow):
        row = [drow.get(field, '') for field in self.fields]
        super(UnicodeDictWriter, self).writerow(row)   


def writeFile(outputData,name="out_playas2.csv"):	
	# create the output file
	out = open(name, 'w')
	 
	print "Writing output file: "+ out.name
	try:
		#fieldnames= outputData[0].keys()
		fieldnames= ['id', 'nombre', 'comunidad','municipio','provincia','lat','lon'];

		writer = UnicodeDictWriter(out,fieldnames)
		headers = dict( (n,n) for n in fieldnames )
		writer.writerow(headers)
		for row in outputData:
			writer.writerow(row)
			
	 
	finally:
	    out.close()	
	    	
 	
myparser = CloginParser();
num=range(1,1945)
num.remove(1927)
i=0;
outputData1=[]   
for index in num:
	#f = open(str(index) + '.html', 'r')
	fileObj = codecs.open( str(index) + '.html', "r", "Latin-1" )
	u = fileObj.read() 
	u=u.encode("utf-8")
	newrow=myparser.parse( u )
	myrow=copy.copy(newrow)
	#myrow.encode("utf-8")		
	if (len( newrow.keys() ) >5 ):
		myrow["id"]=i;
		i+=1;
		outputData1.append(myrow)	
	#print myrow


#print outputData1	
writeFile(outputData1)	
