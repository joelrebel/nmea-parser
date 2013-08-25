struct GGA {
	//$GPGGA,083837.000,19234.5818,N,0755.4262,E,1,3,2.68,212.5,M,-62.5,M,,*74
	//all char arrays are field+1 ('\0')
	char utc_time[11]; //field 1 hhmmss.sss
	char latitude[11]; //field 2 ddmm.mmmm
	char NS[2]; //field 3 N=north or S=south
	char longitude[11]; //field 4 dddmm.mmmm
	char EW[2]; //field 5 E=east or W=west
	char fix[2]; //field 6 Position indicator
	char satellites[3];//field 7 Range 0-14 Antenna Altitude above/below mean-sea-leve
	char altitude[11]; //field 9 Antenna Altitude above/below mean-sea-level in meters
	char geodal_separation[5]; //in meters http://homepages.slingshot.co.nz/~geoff36/datum.htm
};
	
struct VTG {
	//$GPVTG,0.00,T,,M,0.17,N,0.32,K,N*354
	char course[8]; //field 1 in degrees
	char speed_knots[8]; //field 6
	char speed_kmphr[8]; //field 7
	char mode[1]; //field 9
};

int store_gps_word(int field_count, char *gps_word, char *gps_header);
int split_string(char *str);
