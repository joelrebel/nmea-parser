#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_gps.h"


struct GGA gps_data_gga;
struct VTG gps_data_vtg; //initialize VTG struct

int main(void)
{
	int idx=0;
	const char GPS[] = "/opt/msp430/gps/gps_test_dump.cap2";
	char string[120]; 
    char ch;
	int data_ready=0; //1 = gga data is ready; 2 = vtg data is ready

	FILE *in_file;
	in_file = fopen(GPS, "r");
	
	if (in_file == NULL){
		printf("ERRR!\n");
		exit(8);
	}
	
	while(1){

		ch = fgetc(in_file);
		if (ch == EOF){ break; }
		if ( ch == '\r' || ch == '\n') {
			string[idx] = '\0';
//			printf("chars- %i\n", count_chars(string));
			idx=0;
			data_ready=split_string(string);
			if(data_ready){ print_gps_data(data_ready); data_ready=0;}
//			printf("ready = %i\n", data_ready);
			continue;
		}else{	
			string[idx] = ch;
		}
			
		idx++;	
	}

	fclose(in_file);
	return 0;
}

print_gps_data(int data_ready){
	if (data_ready == 1){
			printf("\n:utc_time: %s:\n", gps_data_gga.utc_time);
			printf(":latitude: %s:\n", gps_data_gga.latitude);
			printf(":NS: %s:\n", gps_data_gga.NS);
			printf(":longitude: %s:\n", gps_data_gga.longitude);
			printf(":EW: %s:\n", gps_data_gga.EW);
			printf(":fix: %s:\n", gps_data_gga.fix);
			printf(":satellites: %s:\n", gps_data_gga.satellites);
			printf(":altitude: %s:\n", gps_data_gga.altitude);
			printf(":geodal_separation: %s:\n", gps_data_gga.geodal_separation);
			printf("XXXXXXXXXX\n");
			data_ready=0;
			//reset our struct
			memset(&gps_data_gga, 0, sizeof(gps_data_gga));

		}
		if (data_ready == 2){
			printf("\n:heading/course: %s:\n", gps_data_vtg.course);
			printf(":knots: %s:\n", gps_data_vtg.speed_knots);
			printf(":speed kmhpr: %s:\n", gps_data_vtg.speed_kmphr);
			printf(":mode: %s:\n", gps_data_vtg.mode);
			data_ready=0;

			memset(&gps_data_vtg, 0, sizeof(gps_data_vtg));
		
		}
}
/*
int count_chars(char *str)
{
	int count=0;
	while(*str++){ count++; }
	return count;

}
*/
int split_string(char *str)
{
	char str_start = '$';
	char word_sep = ',';
	char gps_parse_buf[91];
	char *gps_word;

//$GPGGA,000258.037,,,,,0,0,,,M,,M,,*431  
	gps_word = gps_parse_buf;
	int idx=0;
	int grab_char = 0;
	int field_count =0;
	int is_gpgga =0 ;
	int is_gpvtg =0 ;
	char gps_header[5];
	int gga_data_ready=0; 
    int vtg_data_ready=0;
	
	while( str[idx] != '\n' ){
		
		switch(str[idx]) {
				case '$':
					grab_char = 1;
					break;
				
				case ',':
					
					*gps_word++ = '\0'; //end of the word
	//				printf( "gps_word : %d -  %s \n", field_count, gps_parse_buf);
					
					if (field_count == 0){ //work with the header.
						gga_data_ready=0;
						vtg_data_ready=0;

						//printf("%d\n", field_count);
						if (strcmp(gps_parse_buf,"GPGGA") == 0){
							strcpy(gps_header,"GPGGA");
//						    printf("%s ->\n",gps_header);
							is_gpgga =1;
						}else{
							is_gpgga =0;
						}
						if (strcmp(gps_parse_buf,"GPVTG") == 0){
							strcpy(gps_header,"GPVTG");
					//	    printf("%s ->\n",gps_header);
							is_gpvtg =1;
						}else{
							is_gpvtg =0;
						}
					} 

					if ( is_gpgga == 1 ){//if we are in a GPGGA string, store this
	//					printf("%s HERE %d \n", gps_parse_buf, field_count);
				    	if ( field_count > 12){ break; } //for gpgga we only care about the first 9 fields
					    gga_data_ready = store_gps_word(field_count, gps_parse_buf, gps_header);
					//	if(gga_data_ready) { 	printf("\n:longitude: %s - \n", gps_data_gga.longitude); }
						//printf("gga_data_ready - %i\n", gga_data_ready);
					}
					
					if ( is_gpvtg == 1){//if we are in a GPVTG string, store this
	//					printf("%s HERE %d \n", gps_parse_buf, field_count);
						vtg_data_ready = store_gps_word(field_count, gps_parse_buf, gps_header);
						//printf("vtg_data_ready - %i\n", vtg_data_ready);
					}

					gps_word = gps_parse_buf; //reset the pointer
					field_count++; //next field
					break;
					
				default:
					if(grab_char){ *gps_word++ = str[idx];}
					break;
	   }
		if (gga_data_ready) {  return(1); } //lets roll!
		if (vtg_data_ready) {  return(2); } //lets roll!
		idx++;
	}

//	   printf("\nEOL\n");
	return (0);//by default data isnt ready, hence the 0
}

int store_gps_word(int field_count, char *gps_word, char *gps_header)
{
//	printf("hd %s - wd %s - fc %d - sz %d - len %d\n", gps_header, gps_word, field_count, sizeof(gps_word), strlen(gps_word));	
	//return;
	int data_ready=0;	
	if ((strcmp(gps_header, "GPGGA" )) == 0){


		switch(field_count){
				
				case 0:
					//printf("\n:GPGGA header: %s\n", gps_word);
					break;
				
				case 1:
					strcpy(gps_data_gga.utc_time, gps_word);
//					printf("strcpy done\n");

					break;
				
				case 2:
					strcpy(gps_data_gga.latitude, gps_word);

					//printf("\n:utc_time: %s\n", gps_data_gga.utc_time);
					//printf("\n:latitude: %s\n", gps_data_gga.latitude);
					break;
				
				case 3:
					strcpy(gps_data_gga.NS, gps_word);

					//	printf("hd %s - wd %s - fc %d - sz %d - len %d\n", gps_header, gps_word, field_count, sizeof(gps_word), strlen(gps_word));	
					break;
				
				case 4:	
					strcpy(gps_data_gga.longitude, gps_word);

//					printf("\n:NS: %s\n", gps_data_gga.NS);
	//				printf("\n:longitude: %s\n", gps_data_gga.longitude);

					break;
				
				case 5:
					strcpy(gps_data_gga.EW, gps_word);
//					strcpy(gps_data_gga.EW, gps_word);
					//printf("\n:EW: %s\n", gps_data_gga.EW);
					break;
				
				case 6:
					
					strcpy(gps_data_gga.fix, gps_word);
//					strcpy(gps_data_gga.fix, gps_word);
					//printf("\nfix: %s\n", gps_data_gga.fix);
					break;
				
				case 7:
					strcpy(gps_data_gga.satellites, gps_word);
					//printf("\n:satellites: %s\n", gps_data_gga.satellites);
					break;
				
				case 9:
					strcpy(gps_data_gga.altitude, gps_word);
					//printf("\n:altitude: %s\n", gps_data_gga.altitude);
					break;
				
				case 11:
					strcpy(gps_data_gga.geodal_separation, gps_word);
					data_ready=1;
					break;

				default:
					break;
		}//end switchcase
	
	}//end if
	
	if ((strcmp(gps_header, "GPVTG" )) == 0){
		

		switch(field_count){
			
			case 0:

				//printf("\n:header: %s\n", gps_word);
				break;
			
			case 1:
				strcpy(gps_data_vtg.course, gps_word);
				//printf("\n:heading/cource: %s\n", gps_data_vtg.course);
				break;
			
			case 5:
				strcpy(gps_data_vtg.speed_knots, gps_word);
				//printf("\n:knots: %s\n", gps_data_vtg.speed_knots);
				break;

			case 7:		
				strcpy(gps_data_vtg.speed_kmphr, gps_word);
				//printf("\n:speed kmhpr: %s\n", gps_data_vtg.speed_kmphr);
				break;
			
/*			case 9: //not sure if we need this mode data
				strcpy(gps_data_vtg.mode, gps_word);
				//printf("\n:mode: %s\n", gps_data_vtg.mode);
				data_ready=1;
				break;
*/
		}//end switchcase	
	 } // end if
	
//       printf("\n:utc_time: %s\n", gps_data_gga.utc_time);
	return data_ready;
} //end function

