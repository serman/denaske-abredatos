from xml.dom import minidom 
import csv
import sgmllib
from HTMLParser import HTMLParser
import os
import codecs
import cStringIO
import StringIO

class CloginParser(sgmllib.SGMLParser): #La que va entre parentesis es la clase madre. Estamos heredando. 
    def parse(self, s):
        self.feed(s)
        self.close()
        return self.tipo, self.comunidad, self.size, self.nombre
        
 
    def __init__(self, verbose=0):
		sgmllib.SGMLParser.__init__(self, verbose) 
		self.status=""
		self.tipo=""
		self.comunidad=""
		self.size=""
		self.inside=False
        
     # Este parser es parecido a un SAX de xml. La clase madre va llamando a metodos especificos para cada etiqueta que encuentra.
     #En nuestro caso nos interesa cojer del codigo html del formulario los parametros login y hash. 
    def start_td(self, aAttributes):
    	self.inside=True
        return
        
    def end_td(self):
    	self.inside=False;    
            
    def handle_data(self,data):   	
    	if( data=="CCAA"):
    		self.status="ccaa"
    		return
    	elif (data == "Figura"):
    		self.status="figura"
    		return
    	elif (data == "Superficie (ha)"):
    		self.status="superficie"
    		return
    	elif( data=="CCAA"):
    		self.status="ccaa"
    		return	   
    	elif( data=="Nombre"):
    		self.status="nombre"
    		return	   
    		 		
    	if(self.inside==True):
	    	if (self.status=="ccaa"):
	    		self.comunidad=data
	    		self.status=""
	    	elif (self.status=="figura"):
	    		self.tipo=data
	    		self.status=""
	    	elif (self.status == "superficie"):
	    		self.size=data
	    		self.status=""	    	
	    	elif (self.status == "nombre"):
	    		self.nombre=data
	    		self.status=""		    		
    	return


class UnicodeWriter(object):

    def __init__(self, f, dialect=csv.excel, encoding="utf-8", **kwds):
        # Redirect output to a queue
        self.queue = StringIO.StringIO()
        print csv.list_dialects()
        self.writer = csv.writer(self.queue, dialect=dialect, **kwds)
        self.stream = f
        self.encoding = encoding
    
    def writerow(self, row):
        # Modified from original: now using unicode(s) to deal with e.g. ints
        self.writer.writerow([unicode(s).encode("utf-8") for s in row])
        # Fetch UTF-8 output from the queue ...
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
    def __init__(self, f, fields, dialect=csv.excel,
            encoding="utf-8", **kwds):
        super(UnicodeDictWriter, self).__init__(f, dialect, encoding, **kwds)
        self.fields = fields
    
    def writerow(self, drow):
        row = [drow.get(field, '') for field in self.fields]
        super(UnicodeDictWriter, self).writerow(row)
           


class kmlParques():
	def __init__(self):
		self.outputData = []
		return
	
	
	def parse(self,filename):		
		xmldoc = minidom.parse(filename)	
		placemarks=xmldoc.getElementsByTagName("Placemark")	
		i=0;
		imageData = dict();
		self.outputData = []
		myparser = CloginParser();
		id=0;
		for place in placemarks:
			imageData = {}
			imageData['id']=id
			id+=1;
			description=place.getElementsByTagName("description")[0].firstChild.data
			html=myparser.parse(description)
			#        return self.tipo, self.comunidad, self.size
			imageData['name']= html[3] #place.getElementsByTagName("name")[0].firstChild.data
			imageData['size']=html[2]
	
			outerBoundary=place.getElementsByTagName("outerBoundaryIs")[0].firstChild.childNodes[0].childNodes[0].data
			test= outerBoundary.split(" ", 3)
			longlat=test[1].split(",")
			
			imageData['lon']=longlat[0].decode()
			imageData['lat']=longlat[1].decode()
			imageData['tipo']=html[0]
			imageData['comunidad']=html[1]
			
		#Dar la vuelta a longitud latitud por latitud longitud	
			neworder= outerBoundary.split();
			#print neworder
			del neworder[0]
			#print imageData['outerBoundary']
			lat_lon=""
			for tupla in neworder:
			#	print tupla
				lon_lat_alt=tupla.split(",")
			#	print lon_lat_alt
			#	print lon_lat_alt[0]					
				#print lon_lat_alt	
				if (lat_lon==""):	
					lat_lon=lon_lat_alt[1] + "," + lon_lat_alt[0]
				else:
					lat_lon=lat_lon + " + " + lon_lat_alt[1] + "," + lon_lat_alt[0]
				#if(i<=10):
				#	i+=1;
			#print  lat_lon			
			imageData['outerBoundary']=lat_lon.decode()
			#print imageData['area']
			#print name, description,outerBoundary			
			self.outputData.append(imageData)
		
	def writeFile(self,name="out.csv"):		
	# create the output file
		out = open(name, 'w')
	 
		print "Writing output file: "+ out.name
		try:
		    #fieldnames = self.outputData[0].keys()
		    fieldnames= ['id', 'name', 'comunidad','tipo','lon','lat','size','outerBoundary'];
		    #fieldnames.reverse()
		    writer = UnicodeDictWriter(out,fieldnames)
		    
		    headers = dict( (n,n) for n in fieldnames )
		    writer.writerow(headers)
		 
		    for row in self.outputData:
		        writer.writerow(row)
		 
		finally:
		    out.close()	
	
	
myparser=kmlParques()
path = './'
myparser.parse('reservasnaturales.kml')

myparser.writeFile("reservasnaturales.csv")
           
                    

	