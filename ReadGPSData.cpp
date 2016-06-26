#include "ReadGPSData.h"
#include<SerialStream.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

using namespace LibSerial;
    
SerialStream serial_port;            

void gps::calc_bearstr()
{
	double lat1,lat2,lon1,lon2,bear_val;
	lat1=atof(prev_latstr)*deg_to_rad;
	lat2=atof(latstr)*deg_to_rad;
	lon1=atof(prev_longstr)*deg_to_rad;
	lon2=atof(longstr)*deg_to_rad;

  	bear_val=atan2(sin(lon2-lon1)*cos(lat2),(cos(lat1)*sin(lat2)-sin(lat1)*cos(lat2)*cos(lon2-lon1)));
  	bear_val=bear_val*rad_to_deg;
  	
	if(bear_val<180)
  		bear_val+=360;

	snprintf(bearstr,14,"%f",bear_val);	
}

void gps::return_prev()
{
	
    if(!strcmp(latstr,""))		
	strcpy(latstr,prev_latstr);
	if(!strcmp(longstr,""))
	strcpy(longstr,prev_longstr);
	if(!strcmp(altstr,""))	
	strcpy(altstr,prev_altstr);
	if(!strcmp(utctime,""))
	strcpy(utctime,prev_utctime);	
	if(!strcmp(bearstr,""))
	calc_bearstr();
}

int gps::initport(char s[20])   //Initializes a port (sets baud, parity, char_size, stop_bits) and returns flag
	{
        serial_port.Open(s);   //Open port
        if(serial_port.good())
		{
			serial_port.SetBaudRate( SerialStreamBuf::BAUD_9600 ) ;  //Set Baud rate 9600
            if ( ! serial_port.good() )
   			 {
				printf("\n\nCould Not Set Baud\n\n");
        		return 0;
             }
    		 
    		serial_port.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 ) ;  //Set Char Size 8
            if ( ! serial_port.good() )
			{
        	printf("\n\nCould Not Set  Char Size\n\n");
       	 	return 0 ;
            }
    		
    		serial_port.SetParity( SerialStreamBuf::PARITY_NONE ) ;  //Set Parity None
            if ( ! serial_port.good() )
    		{
        	printf("\n\nCould Not Set Parity\n\n");
        	return 0 ;
            }

	    	serial_port.SetNumOfStopBits( 1 ) ;  //Set Stop bits 1
            if ( ! serial_port.good() )
    		{
        	printf("\n\nCould Not Set Stop bits\n\n");
        	return 0;
            }

		    serial_port.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE ) ;  //Set Flow Control None
            if ( ! serial_port.good() )
    		{
      		printf("\n\nCould Not Set Hardware Flow Control To Off\n\n");
        	return 0;
            }

	    	// Wait for some data to be available at the serial port.
    		//
   			 
			if( serial_port.rdbuf()->in_avail() == 0 )      
   			{
        		usleep(100000) ;
    		}
    	
            if(!verifyport())  //Verify port
			{
      		printf("\n\nCould Not Verify Port\n\n");
        	return 0 ;
            }
            return verifyport();

		}
        else if(!serial_port.good())
		{
            serial_port.Close();
			return 0;
        }
        else
        return 0;
	}    
	
	int gps::findport()
	{
		
		for(int i=0;i<4;i++)
		{
			int flag=0;
			flag=initport(defined_ports[i]);
			if(flag)
			{
			strcpy(port,defined_ports[i]);
			port_status=1;
			return 1;
			}
		}
		port_status=0;
		return 0;
	}
	
	int gps::verifyport()
	{
		char next_byte;
        for(int j=0;j<65000;j++)
        {
        
    	serial_port.get(next_byte);
    	if(next_byte=='$')
    		{
    			serial_port.get(next_byte);
				if(next_byte=='G')
				{
					serial_port.get(next_byte);
					if(next_byte=='P')
						return 1;
						
				}
    		}
    		
    	}
    	return 0;
	}
	
	int gps::openport()
	{
		return findport();
		
	}
	
	int gps::openport(char s[20])
	{
        serial_port.Open(s);
		if ( ! serial_port.good() ) 
    		{
        	printf("\n\nCould Not Open Port\n\n");
        	return 0;
    		
			}
		else return 0;
	}
	
	void gps::closeport()
	{
        serial_port.Close();
	}
	
	
	int gps::update()
	{

        if(findport())
        {
	        char buff[100];
	        for(int a=0;a<100;a++)
	            buff[a]=0;
	        int j=1,counter=0,counter2=0;
	
	        for(int i=0; i<5;i++) 
	        {
				if(serial_port.rdbuf()->in_avail()==0)
				 {
		            usleep(1000000);
		         }
		
				 while( serial_port.rdbuf()->in_avail() > 0  ) 
		    		{
		        	  char next_byte;
		        	  serial_port.get(next_byte);
		        	  if(next_byte=='$')
		                {
		                  buff[0]='$';j=1;
		                  while(buff[j-4]!='*')
		        			{
		        			  serial_port.get(next_byte);
		                      buff[j]=next_byte;
		        			  j++;  			  
		        			}
		        	  	
		        		   if(extract(buff))
		                   {
		                       closeport();
					   		   return 1;
		                   }
		        		}
		              else
		        		counter2++;
		
		                if(counter2==100)
		                {
		                    closeport();
		                    return_prev();
		                    return 1;
		                }
		        		
					}
		            
		
		        	 
		        
        	}

        closeport();
        return_prev();
        return 1;
        }
        else
        {
            return_prev();
			return 1;
            
        }

    }
	
	
	
	int gps::extract(char a[100])
{
        char temp[100];
        for(int aj=0;aj<100;aj++)
            temp[aj]=0;
        strcpy(temp,a);
        if(temp[0]=='$')
        {
           int i=0;
            token =strtok (temp,",$*");
            while (token != NULL)
            {
                sprintf(data[i],"%s",token);
                i++;
                token = strtok (NULL,",$*");
            }
            if(setvalues()==1)
			return 1;
			return 0;
	}
}


int gps::setvalues()
{
	
	if(strcmp(data[GPSFORMAT],"GPGGA")==0)
    {
        
		strcpy(prev_latstr,latstr);
        strcpy(prev_longstr,longstr);
        strcpy(prev_altstr,altstr);
        strcpy(prev_latitudehem,latitudehem);
        strcpy(prev_longitudehem,longitudehem);
        strcpy(prev_utctime,utctime);
         
		strcpy(latstr,data[LATITUDE]);
        strcpy(altstr,data[ALTITUDE]);
        strcpy(longstr,data[LONGITUDE]);
        strcpy(latitudehem,data[LATITUDEHEM]);
        strcpy(longitudehem,data[LONGITUDEHEM]);
        strcpy(utctime,data[UTCTIME]);
        
		
    	if(!(strcmp(latstr,"")||strcmp(longstr,"")||strcmp(altstr,"")||strcmp(bearstr,"")||strcmp(utctime,"")))
		return_prev();
		
		return 1;
    }
    else if(strcmp(data[GPSFORMAT],"GPRMC")==0)
    {
        strcpy(prev_latstr,latstr);
        strcpy(prev_longstr,longstr);
        strcpy(prev_latitudehem,latitudehem);
        strcpy(prev_longitudehem,longitudehem);
        strcpy(prev_utctime,utctime);
        strcpy(prev_bearstr,bearstr);
		
		strcpy(latstr,data[RLATITUDE]);
        strcpy(bearstr,data[RBEARING]);
        strcpy(longstr,data[RLONGITUDE]);
        strcpy(latitudehem,data[RLATITUDEHEM]);
        strcpy(longitudehem,data[RLONGITUDEHEM]);
        strcpy(utctime,data[RUTCTIME]);
        
    	if(!(strcmp(latstr,"")||strcmp(longstr,"")||strcmp(altstr,"")||strcmp(bearstr,"")||strcmp(utctime,"")))
		return_prev();
		
		return 2;
    }
    return -1;
}






/*int main()
{
    gps g;
    g.findport();
    for(int i=0;i<50;i++)
	{
	    if(g.update())
	   {
	        printf("\n%sA",g.latstr);
	        printf("\n%sA",g.longstr);
	        printf("\n%sA",g.bearstr);
	        printf("\n%sA",g.altstr);
	   }
	   
	   serial_port.Close();
	}
	return 0;
}
*/

