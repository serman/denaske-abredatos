from xml.dom import minidom 
import csv
import sgmllib
from HTMLParser import HTMLParser
import os
import codecs
import cStringIO
import StringIO
import os
import glob


class CloginParser(sgmllib.SGMLParser): #La que va entre parentesis es la clase madre. Estamos heredando. 
    def parse(self, s):
        self.feed(s)
        self.close()
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
    	if( data=="Provincia"):
    		self.status="provincia"
    		return
    	elif (data == "Tipo de firme"):
    		self.status="tipo"
    		return
    	elif (data == "Longitud en km"):
    		self.status="longitud"
    		return    		
    	elif (data == "Recorrido"):
    		self.status="recorrido"
    		return   
    	
    	elif (data == "Nombre"):
    		self.status="nombre"
    		return   	
    			
    	if(self.inside==True):
	    	if (self.status=="provincia"):
	    		self.mydict["provincia"]=data
	    		self.status=""
	    	elif (self.status=="tipo"):
	    		self.mydict["tipo"]=data
	    		self.status=""
	    	elif (self.status == "longitud"):
	    		self.mydict["longitud"]=data
	    		self.status=""	 
	    	elif (self.status == "nombre"):
	    		self.mydict["nombre"]=data
	    		self.status=""		 
	    	elif (self.status == "recorrido"):
	    		self.mydict["recorrido"]=data
	    		self.status=""	  	
    	return


class UnicodeWriter(object):

    def __init__(self, f, dialect=csv.excel, encoding="utf-8", **kwds):
        # Redirect output to a queue
        self.queue = StringIO.StringIO()
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

class kmlVias():
	def __init__(self):
		self.outputData = []
		self.i=0;
		return

	def parsekml(self,filename):
		xmldoc = minidom.parse(filename)
		
		placemarks=xmldoc.getElementsByTagName("Placemark")
		
		#imageData = dict();
		myparser = CloginParser();
		
		for place in placemarks:
			#if(i==0):
				#imageData = {}
				description=place.getElementsByTagName("description")[0].firstChild.data
				imageData=myparser.parse(description)
				#        return self.tipo, self.comunidad, self.size				
				imageData['outerBoundary']=place.getElementsByTagName("coordinates")[0].firstChild.data
				test= imageData['outerBoundary'].split(" ", 3)
				longlat=test[0].split(",")
				imageData['lon']=longlat[0].decode()
				imageData['lat']=longlat[1].decode()
				imageData['id']=self.i;
				self.i+=1;
#----				
				neworder= imageData['outerBoundary'].split();
				#print neworder
				#del neworder[0]
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
				self.outputData.append(imageData)#------				
				
								
			#	print imageData['lon'], " - ",  imageData['lat']
				#print name, description,outerBoundary
				#i+=1;
			
		
	def writeFile(self,name="viasverdes_comas.csv"):	
		# create the output file
		out = open(name, 'w')
		 
		print "Writing output file: "+ out.name
		try:
			fieldnames= self.outputData[0].keys()
			writer = UnicodeDictWriter(out,fieldnames)
			headers = dict( (n,n) for n in fieldnames )
			writer.writerow(headers)
			for row in self.outputData:
				writer.writerow(row)
		 
		finally:
		    out.close()	
	
	
# this is what is actually called
myparser=kmlVias()
path = './'
for infile in glob.glob( os.path.join(path, '*.kml') ):
	print "current file is: " + infile
	myparser.parsekml(infile)

myparser.writeFile()

           
                    

	