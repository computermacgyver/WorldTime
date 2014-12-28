//Declare and Import references
#include "pebble.h"
#include "pebble_fonts.h"


/*
//Watchface header section
#define MY_UUID { 0xE9, 0x96, 0x5A, 0xC7, 0x5E, 0xA1, 0x49, 0xC1, 0xA6, 0xC3, 0x05, 0xAD, 0xA8, 0x4F, 0xBF, 0x2A }

// Spanish: 4296c359-6177-4d96-b1a4-262bf90b7af9
// English: 4e2ae2a7-ad7e-4955-a228-13ec4179dd35


PBL_APP_INFO(MY_UUID,
             "World Time", "dabdemon", 
             1, 0, // App version
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);
*/		 
#define MyTupletCString(_key, _cstring) ((const Tuplet) { .type = TUPLE_CSTRING, .key = _key, .cstring = { .data = _cstring, .length = strlen(_cstring) + 1 }})

#define WEEKDAY_FRAME	  (GRect(5,  2, 95, 168-145)) 
#define BATT_FRAME 	      (GRect(100,  4, 40, 168-146)) 
#define BT_FRAME 	      (GRect(125,  4, 25, 168-146)) 
#define TIME_FRAME        (GRect(0, 15, 144, 168-16)) 
#define DATE_FRAME        (GRect(1, 69, 139, 168-62)) 

#define WC1NAME_FRAME     (GRect(5,  102, 105, 168-145))//95
#define WC1TIME_FRAME     (GRect(100, 102, 41, 168-145))
	
#define WC2NAME_FRAME     (GRect(5,  117, 105, 168-145))//110
#define WC2TIME_FRAME     (GRect(100, 117, 41, 168-145))
	
#define WC3NAME_FRAME     (GRect(5,  132, 105, 168-145)) //125
#define WC3TIME_FRAME     (GRect(100, 132, 41, 168-145))
	
//#define WC4NAME_FRAME     (GRect(5,  140, 105, 168-145))
//#define WC4TIME_FRAME     (GRect(100, 140, 41, 168-145))


//Declare initial window	
	Window *my_window;    

//Define the layers
	TextLayer *date_layer;   		// Layer for the date
	TextLayer *Time_Layer; 			// Layer for the time
	TextLayer *Weekday_Layer; 		//Layer for the weekday
	TextLayer *Last_Update; 		// Layer for the last update
	TextLayer *Location_Layer; 		// Layer for the last update
	TextLayer *Batt_Layer;			//Layer for the BT connection
	TextLayer *BT_Layer;			//Layer for the BT connection
	TextLayer *WC1NAME_Layer;	//Layer for the Temperature
	TextLayer *WC1TIME_Layer;	//Layer for the Temperature
	TextLayer *WC2NAME_Layer;	//Layer for the Temperature
	TextLayer *WC2TIME_Layer;	//Layer for the Temperature
	TextLayer *WC3NAME_Layer;	//Layer for the Temperature
	TextLayer *WC3TIME_Layer;	//Layer for the Temperature
//	TextLayer *WC4NAME_Layer;	//Layer for the Temperature
//	TextLayer *WC4TIME_Layer;	//Layer for the Temperature

/*
	TextLayer *Max_Layer;			//Layer for the Max Temperature
	TextLayer *Min_Layer;			//Layer for the Min Temperature
	*/
	static GBitmap *BT_image=NULL;
	static BitmapLayer *BT_icon_layer; //Layer for the BT connection

	static GBitmap *Batt_image=NULL;
	static BitmapLayer *Batt_icon_layer; //Layer for the Battery status


//Define and initialize variables
	//FONTS
	GFont font_date;        // Font for date
	GFont font_time;        // Font for time
	GFont font_update;      // Font for last update
	GFont font_temperature;	// Font for the temperature
	GFont font_cj;			// Font for Chinese, Japanese

	//Vibe Control
	bool BTConnected = false;

	//Date & Time	
	static char last_update[]="00:00 ";
	//static int initial_minute; //Not used -- SAH

	static char weekday_text[] = "          ";
	static char date_text[] = "XXX 00";
	static char month_text[] = "             ";
	static char day_text[] = "31";
	static char day_month[]= "31 SEPTEMBER"; 
	static char time_text[] = "00:00"; 

	//WORLD CLOCK
	int intLocalTime  = 0;
	int local_hours= 0;
	int local_min=0;
	int TZ_min=0;

	static char tz1_name[]="             ";
	int intTZ1  = 0;
	int tz1_hours= 0;
	int tz1_min=0;
	static char tz2_name[]="             ";
	int intTZ2  = 0;
	int tz2_hours= 0;
	int tz2_min=0;
	static char tz3_name[]="             ";
	int intTZ3  = 0;
	int tz3_hours= 0;
	int tz3_min=0;
	
	static char TZ1[] = "00:00";
	static char TZ2[] = "00:00";
	static char TZ3[] = "00:00";

	bool translate_sp = true;
	//static char language[] = "E";//Never used --SAH
	int intLanguage = 100;
	bool color_inverted = false;
	bool blninverted =  false;

	InverterLayer *inv_layer;


enum TimeZoneKey {
  Language_KEY = 0x0,        // TUPLE_INT
  LocalTime_KEY = 0x1,        // TUPLE_INT
  TZ1Name_KEY = 0x2,        // TUPLE_CSTRING 
  TZ1Time_KEY = 0x3,      // TUPLE_INT
  TZ2Name_KEY = 0x4,        // TUPLE_CSTRING 
  TZ2Time_KEY = 0x5,      // TUPLE_INT
  TZ3Name_KEY = 0x6,        // TUPLE_CSTRING 
  TZ3Time_KEY = 0x7,      // TUPLE_INT
  INVERT_COLOR_KEY = 0x8,  // TUPLE_INT
};

static const uint32_t TimeZones[] = {
	-12,//(GMT -12:00) Eniwetok, Kwajalein
	-11,//(GMT -11:00) Midway Island, Samoa
	-10,//(GMT -10:00) Hawaii
	-9,// (GMT -9:00) Alaska
	-8,// (GMT -8:00) Pacific Time (US & Canada)
	-7,// (GMT -7:00) Mountain Time (US & Canada)
	-6,// (GMT -6:00) Central Time (US & Canada), Mexico City
	-5,// (GMT -5:00) Eastern Time (US & Canada), Bogota, Lima
	-4,// (GMT -4:30) Caracas
	-4,// (GMT -4:00) Atlantic Time (Canada), La Paz, Santiago
	-3,// (GMT -3:30) Newfoundland
	-3,// (GMT -3:00) Brazil, Buenos Aires, Georgetown
	-2,// (GMT -2:00) Mid-Atlantic
	-1,// (GMT -1:00 hour) Azores, Cape Verde Islands
	0, // (GMT) Western Europe Time, London, Lisbon, Casablanca
	1, // (GMT +1:00 hour) Brussels, Copenhagen, Madrid, Paris
	2, // (GMT +2:00) South Africa, Cairo
	3, // (GMT +3:00) Baghdad, Riyadh, Kaliningrad
	3, // (GMT +3:30) Tehran
	4, // (GMT +4:00) Abu Dhabi, Muscat, Yerevan, Baku, Tbilisi, Moscow, St. Petersburg
	4, // (GMT +4:30) Kabul
	5, // (GMT +5:00) Islamabad, Karachi, Tashkent
	5, // (GMT +5:30) Mumbai, Kolkata, Chennai, New Delhi
	5, // (GMT +5:45) Kathmandu
	6, // (GMT +6:00) Almaty, Dhaka, Colombo, Ekaterinburg
	6, // (GMT +6:30) Yangon, Cocos Islands
	7, // (GMT +7:00) Bangkok, Hanoi, Jakarta
	8, // (GMT +8:00) Beijing, Perth, Singapore, Hong Kong
	9, // (GMT +9:00) Tokyo, Seoul, Osaka, Sapporo
	9, // (GMT +9:30) Adelaide, Darwin
	10,// (GMT +10:00) Eastern Australia, Guam, Yakutsk
	11,// (GMT +11:00) Magadan, Solomon Islands, New Caledonia, Vladivostok
	12,// (GMT +12:00) Auckland, Wellington, Fiji, Kamchatka
};

static const char *WEEKDAYS[] = {
	NULL,
	//SPANISH - 0
	"Lunes",
	"Martes",
	"Miércoles",
	"Jueves",
	"Viernes",
	"Sábado", 
	"Domingo", 
	//ITALIAN - 1
	"Lunedi",
	"Martedi",
	"Mercoledi", 
	"Giovedi", 
	"Venerdi", 
	"Sabato", 
	"Domenica", 
	//GERMAN - 2
	"Montag", 
	"Dienstag", 
	"Mittwoch", 
	"Donnerstag", 
	"Freitag", 
	"Samstag", 
	"Sonntag",
	//CZECH - 3
	"Pondělí",
	"Úterý", 
	"Streda", 
	"Čtvrtek", 
	"Pátek", 
	"Sobota", 
	"Neděle", 
	//FRENCH - 4
	"Lundi",
	"Mardi", 
	"Mercredi",
	"Jeudi", 
	"Vendredi", 
	"Samedi", 
	"Dimanche", 
	//PORTUGUESE - 5
	"Segunda", 
	"Terça", 
	"Quarta",
	"Quinta", 
	"Sexta", 
	"Sábado", 
	"Domingo", 
	//FINNISH - 6
	"Maanantai", 
	"Tiistai", 
	"Keskiviikko",
	"Torstai", 
	"Perjantai",
	"Lauantai",
	"Sunnuntai", 
	//DUTCH - 7
	"Maandag", 
	"Dinsdag", 
	"Woensdag", 
	"Donderdag",
	"Vrijdag", 
	"Zaterdag", 
	"Zondag", 
	//POLISH - 8
	"Poniedzialek",
	"Wtorek", 
	"Sroda", 
	"Czwartek",
	"Piątek", 
	"Sobota",
	"Niedziela",
	//SWEDISH - 9
	"Måndag",
	"Tisdag", 
	"Onsdag", 
	"Torsdag", 
	"Fredag", 
	"Lördag",
	"Söndag ",
	//DANISH - 10
	"Mandag",
	"Tirsdag",
	"Onsdag",
	"Torsdag",
	"Fredag", 
	"Lørdag",
	"Søndag ",
	//CATALAN - 11
	"Dilluns", 
	"Dimarts",
	"Dimecres", 
	"Dijous", 
	"Divendres",
	"Dissabte",
	"Diumenge ",
	//HUNGARIAN - 12
	"Hétfo",
	"Kedd", 
	"Szerda",
	"Csütörtök",
	"Péntek", 
	"Szombat",
	"Vasárnap", 
	//NORWEGIAN - 13
	"Mandag",
	"Tirsdag",
	"Onsdag",
	"Torsdag",
	"Fredag", 
	"Lørdag",
	"Søndag ",
};
static const char *WEEKDAYS_JA[]={
	//Japanese - 98
	"月",
	"火",
	"水",
	"木",
	"金",
	"土",
	"日"
};
static const char *WEEKDAYS_ZH[]={
	//Chinese - 99
	"ー",
	"二",
	"三",
	"四",
	"五",
	"六",
	"日"	
};

static const char *MONTHS[] = {
	NULL,
	 //SPANISH - 0
	" enero",
	" febrero",
	" marzo",
	" abril",
	" mayo",
	" junio",
	" julio",
	" agosto", 
	" septiembre",
	" octubre", 
	" noviembre", 
	" diciembre", 
	//ITALIAN - 1
	" gennaio", 
	" febbraio",
	" marzo",
	" aprile",
	" maggio",
	" giugno",
	" luglio",
	" agosto",
	" settembre",
	" ottobre",
	" novembre",
	" dicembre",
	//GERMAN - 2
	".Januar",
	".Februar",
	".März",
	".April",
	".Mai", 
	".Juni", 
	".Juli", 
	".August",
	".September",
	".Oktober",
	".November",
	".Dezember",
	//CZECH - 3
	"Leden ",
	"Únor ",
	"Brezen ",
	"Duben ", 
	"Květen ", 
	"Červen ",
	"Červenec ", 
	"Srpen ",
	"Zárí ",
	"Ríjen ",
	"Listopad ",
	"Prosinec ",
	//FRENCH - 4
	" janvier",
	" février", 
	" mars", 
	" avril", 
	" mai", 
	" juin", 
	" juillet", 
	" août", 
	" septembre", 
	" octobre",
	" novembre", 
	" décembre", 
	//PORTUGUESE - 5
	" Janeiro", 
	" Fevereiro", 
	" Março", 
	" Abril",
	" Maio",
	" Junho",
	" Julho",
	" Agosto",
	" Setembro",
	" Outubro",
	" Novembro",
	" Dezembro",
	//FINNISH - 6
	". Tammikuu",
	". Helmikuu", 
	". Maaliskuu",
	". Huhtikuu", 
	". Toukokuu", 
	". Kesäkuu", 
	". Heinäkuu", 
	". Elokuu", 
	". Syyskuu", 
	". Lokakuu", 
	". Marraskuu", 
	". Joulukuu", 
	//DUTCH - 7
	" Januari", 
	" Februari",
	" Maart",
	" April",
	" Mei", 
	" Juni", 
	" Juli", 
	" Augustus", 
	" September", 
	" Oktober", 
	" November",
	" December", 
	//POLISH - 8
	" stycznia",
	" lutego", 
	" marca", 
	" kwietnia", 
	" maja", 
	" czerwca",
	" lipca", 
	" sierpnia",
	" wrzesnia",
	" pazdziernika",
	" listopada", 
	" grudnia", 
	//SWEDISH - 9
	" Januari", 
	" Februari",
	" Mars", 
	" April", 
	" Maj", 
	" Juni",
	" Juli", 
	" Augusti",
	" September", 
	" Oktober",
	" November", 
	" December", 
	//DANISH - 10
	". Januar",
	". Februar", 
	". Marts", 
	". April", 
	". Maj", 
	". Juni", 
	". Juli",
	". August",
	". September", 
	". Oktober", 
	". November",
	". December",
	//CATALAN - 11
	" Gener", 
	" Febrer", 
	" Març", 
	" Abril", 
	" Maig", 
	" Juny",
	" Juliol", 
	" Agost", 
	" Setembre", 
	" Octubre", 
	" Novembre", 
	" Desembre", 
	//HUNGARIAN - 12
	"január ", 
	"február ", 
	"március ",
	"április ", 
	"május ", 
	"június ", 
	"július ", 
	"augusztus ", 
	"szeptember ", 
	"október ",
	"november ", 
	"december ", 
	//NORWEGIAN - 13
	". januar", 
	". februar", 
	". mars", 
	". april", 
	". mai", 
	". juni", 
	". juli", 
	". august", 
	". september", 
	". oktober", 
	". november", 
	". desember", 
};

//**************************//
// Check the Battery Status //
//**************************//
//TODO: Optimize this. Consider text readout of battery
static void handle_battery(BatteryChargeState charge_state) {
          static char battery_text[] = "100%";
	
	//kill previous batt_image to avoid invalid ones.
	if (Batt_image!=NULL) {
		gbitmap_destroy(Batt_image);
		Batt_image=NULL;
		bitmap_layer_set_bitmap(Batt_icon_layer, NULL);
	}
    

  if (charge_state.is_charging) {
    //snprintf(battery_text, sizeof(battery_text), "charging");
			  Batt_image = gbitmap_create_with_resource(RESOURCE_ID_BATT_CHAR); 
              bitmap_layer_set_bitmap(Batt_icon_layer, Batt_image);
  } else {
	  //snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);

	  
	  //WHILE RUNNING LOW, BATT STATUS WILL ALWAYS DISPLAY  
         //set the new batt_image
         //DO NOT display the batt_icon all the time. it is annoying.
         if (charge_state.charge_percent <=10) //If the charge is between 0% and 10%
         {
			 Batt_image = gbitmap_create_with_resource(RESOURCE_ID_BATT_EMPTY);
             bitmap_layer_set_bitmap(Batt_icon_layer, Batt_image);
         }
	}
}

//******************************//
// Handle Bluetooth Connection  //
//*****************************//
static void handle_bluetooth(bool connected) 
{
  	//text_layer_set_text(BT_Layer, connected ? "C" : "D");

	//draw the BT icon if connected

	if(connected && !BTConnected) {
		if (BT_image!=NULL) {
			gbitmap_destroy(BT_image);
		}
		BT_image = gbitmap_create_with_resource(RESOURCE_ID_BT_CONNECTED);
        bitmap_layer_set_bitmap(BT_icon_layer, BT_image);
		//Vibes to alert connection
		//vibes_double_pulse(); //That's annoying --SAH
		BTConnected = true;
	} else if (!connected && BTConnected) {
		 //Kill the previous image
	    if (BT_image!=NULL) {
	    	gbitmap_destroy(BT_image);
	    	BT_image=NULL;
	    }
		bitmap_layer_set_bitmap(BT_icon_layer, NULL);
		//Vibes to alert disconnection
		//vibes_long_pulse(); //That's annoying -- SAH
		BTConnected = false;
	}


} //handle_bluetooth


//Invert colors
void InvertColors(bool inverted)
{
	
	if (inverted){
		//Inverter layer
		if (blninverted == false){		
			inv_layer = inverter_layer_create(GRect(0, 0, 144, 168));
			layer_add_child(window_get_root_layer(my_window), (Layer*) inv_layer);
			blninverted =  true;
	    }
	}
	
	else{
		if(blninverted){
			inverter_layer_destroy(inv_layer);
			blninverted = false;}
	}

	
}// END - Inver colors


//**************************//
//** Get the current date **//
//**************************//
void getDate()
{
	//Get the date
	time_t actualPtr = time(NULL);
	struct tm *tz1Ptr = gmtime(&actualPtr);
		
	//Try new translation method
		
		//Get the number of the weekday
		strftime(weekday_text,sizeof(weekday_text),"%u",tz1Ptr);
		int ia = weekday_text[0] - '0'; 
		int ib = (intLanguage*7)+ia;
	
		//Get the number of the month	
		strftime(month_text,sizeof(month_text),"%m",tz1Ptr);
		int ic = month_text[1] - '0';
		if (month_text[0]=='1'){ic=ic+10;}			
		int id = (intLanguage*12)+ic;
	
	if(intLanguage==100||intLanguage==101){ //ENGLISH (100)
		
		//remove the chinese week day
		//if (chinese_day) {gbitmap_destroy(chinese_day);}
		//bitmap_layer_set_bitmap(chinese_day_layer, NULL);
		
		if (intLanguage==100) {
			//Get the English (US) format - 100
			strftime(month_text,sizeof(month_text),"%B %e",tz1Ptr);
		} else {
			//Get the English (UK) format - 101
			strftime(month_text,sizeof(month_text),"%e %B",tz1Ptr);
		}
		strftime(weekday_text,sizeof(weekday_text),"%A",tz1Ptr);
		
		text_layer_set_text(Weekday_Layer,weekday_text); //Update the weekday layer  
		text_layer_set_text(date_layer,month_text); 
		
	}
	else if (intLanguage==98||intLanguage==99){//	Japanese/Chinese
		
		//Work on retrieving the correct weekday
		//Get the Month
		strftime(month_text,sizeof(month_text),"%m月%d日",tz1Ptr);
		
		//Clean un the text layer
		//text_layer_set_text(Weekday_Layer,"");
		if (intLanguage==98) {
			//text_layer_set_text(Weekday_Layer, WEEKDAYS_JA[ia-1]); //Japanese weekday
			text_layer_set_text(Weekday_Layer,"");
			
			//Put the Japanese weekday in parenthese after the date.
			//concat(dest,src) src to the end of the string dest and null terminates dest.
			char tmp_weekday[]="（月）";
			snprintf(tmp_weekday,sizeof(tmp_weekday),"（%s）",WEEKDAYS_JA[ia-1]);
			strcat(month_text,tmp_weekday);
		} else {
			char tmp_weekday[]="星期一";
			snprintf(tmp_weekday,sizeof(tmp_weekday),"星期%s",WEEKDAYS_ZH[ia-1]);
			text_layer_set_text(Weekday_Layer, tmp_weekday); //Chinese weekday
		}
		
		/*if (chinese_day) {gbitmap_destroy(chinese_day);}
		chinese_day = gbitmap_create_with_resource(CHINESE_DAYS[ia-1]);
		//Display the weekday in chinese
		bitmap_layer_set_bitmap(chinese_day_layer, chinese_day);*/
		text_layer_set_text(date_layer, month_text);
		
	}
	

	else{
		//remove the chinese week day
		//if (chinese_day) {gbitmap_destroy(chinese_day);}
		//bitmap_layer_set_bitmap(chinese_day_layer, NULL);

		//Set the weekeday
		text_layer_set_text(Weekday_Layer, WEEKDAYS[ib]); //Update the weekday layer  
		
		
		//Get the day
		strftime(day_month,sizeof(day_month),"%e",tz1Ptr);
		
		//Set the month
		//text_layer_set_text(date_layer, MONTHS[id]);
				 if ((intLanguage == 12)||(intLanguage == 3)){
					memcpy(&month_text, MONTHS[id], strlen(MONTHS[id])+1);
					text_layer_set_text(date_layer,strncat(month_text,day_month,strlen(month_text)));} //Czech or Hungarian
				else{text_layer_set_text(date_layer,strncat(day_month,MONTHS[id],strlen(MONTHS[id]))); }
		
	}



}

//TODO: Simplify this code! -- SAH
void getTimeZones(){
	
	time_t actualPtr = time(NULL);

			//Define and Calculate Time Zones
			//TIME ZONE 1
				struct tm *tz1Ptr = gmtime(&actualPtr);
				tz1Ptr->tm_hour += tz1_hours;
				tz1Ptr->tm_min += tz1_min;
		
				//try to fix the timezone when half and hour diff
				if (tz1Ptr->tm_min >=60){
					tz1Ptr->tm_hour = 1 + tz1Ptr->tm_hour;
					tz1Ptr->tm_min = tz1Ptr->tm_min - 60;
				}
		
				//try to fix the timezone when half and hour diff
				if (tz1Ptr->tm_min <0){
					tz1Ptr->tm_hour = tz1Ptr->tm_hour - 1;
					tz1Ptr->tm_min = 60 + tz1Ptr->tm_min;
				}

				//try to fix the timezone when negative
		
				if (tz1Ptr->tm_hour <0){
					tz1Ptr->tm_hour = 24 + tz1Ptr->tm_hour;
				}
		
				//try to fix the timezone when more than 24
				if (tz1Ptr->tm_hour >=24){
					tz1Ptr->tm_hour = tz1Ptr->tm_hour - 24;
				}

				if (clock_is_24h_style()){strftime(TZ1, sizeof(TZ1), "%H:%M", tz1Ptr);}
				else {strftime(TZ1, sizeof(TZ1), "%I:%M", tz1Ptr);}
			//TIME ZONE 2
				struct tm *tz2Ptr = gmtime(&actualPtr);
				tz2Ptr->tm_hour += tz2_hours;
				tz2Ptr->tm_min += tz2_min;
		
				//try to fix the timezone when half and hour diff
				if (tz2Ptr->tm_min >=60){
					tz2Ptr->tm_hour = 1 + tz2Ptr->tm_hour;
					tz2Ptr->tm_min = tz2Ptr->tm_min - 60;
				}
		
				//try to fix the timezone when half and hour diff
				if (tz2Ptr->tm_min <0){
					tz2Ptr->tm_hour = tz2Ptr->tm_hour - 1;
					tz2Ptr->tm_min = 60 + tz2Ptr->tm_min;
				}

				//try to fix the timezone when negative
				if (tz2Ptr->tm_hour <0){
					tz2Ptr->tm_hour = 24 + tz2Ptr->tm_hour;
				}
		
				//try to fix the timezone when more than 24
				if (tz2Ptr->tm_hour >=24){
					tz2Ptr->tm_hour = tz2Ptr->tm_hour - 24;
				}

				if (clock_is_24h_style()){strftime(TZ2, sizeof(TZ2), "%H:%M", tz2Ptr);}
				else{strftime(TZ2, sizeof(TZ2), "%I:%M", tz2Ptr);}
			//TIME ZONE 3		
				struct tm *tz3Ptr = gmtime(&actualPtr);
				tz3Ptr->tm_hour += tz3_hours;
				tz3Ptr->tm_min += tz3_min;
		
				//try to fix the timezone when half and hour diff
				if (tz3Ptr->tm_min >=60){
					tz3Ptr->tm_hour = 1 + tz3Ptr->tm_hour;
					tz3Ptr->tm_min = tz3Ptr->tm_min - 60;
				}
		
				//try to fix the timezone when half and hour diff
				if (tz3Ptr->tm_min <0){
					tz3Ptr->tm_hour = tz3Ptr->tm_hour - 1;
					tz3Ptr->tm_min = 60 + tz3Ptr->tm_min;
				}

				//try to fix the timezone when negative
				if (tz3Ptr->tm_hour <0){
					tz3Ptr->tm_hour = 24 + tz3Ptr->tm_hour;
				}

				//try to fix the timezone when more than 24
				if (tz3Ptr->tm_hour >=24){
					tz3Ptr->tm_hour = tz3Ptr->tm_hour - 24;
				}
		
				if (clock_is_24h_style()){strftime(TZ3, sizeof(TZ3), "%H:%M", tz3Ptr);}
				else{strftime(TZ3, sizeof(TZ3), "%I:%M", tz3Ptr);}
		

			//DISPLAY THE TIME ZONES	
			text_layer_set_text(WC1NAME_Layer, tz1_name);
			text_layer_set_text(WC1TIME_Layer, TZ1);

			text_layer_set_text(WC2NAME_Layer, tz2_name);
			text_layer_set_text(WC2TIME_Layer, TZ2);

			text_layer_set_text(WC3NAME_Layer, tz3_name);
			text_layer_set_text(WC3TIME_Layer, TZ3);
}
/*********************************************************************************/
/* Calculate the difference in hours and minutes between the local and dual zone */
/*********************************************************************************/
void CalculateTimeZone(int LocalZone, int TimeZone, int GMT) {
	
	//Get the Local hours
	local_hours= TimeZones[LocalZone];
	local_min=0;

	//Adjust the minutes
	//LocalZone
	if ((LocalZone == 8)||(LocalZone == 10)){local_min=-30;}
	else if ((LocalZone == 18)||(LocalZone == 20)||(LocalZone == 22)||(LocalZone == 25)||(LocalZone == 29)){local_min=30;}
	else if (LocalZone == 23){local_min=45;}
	//TimeZone
	TZ_min = 0;
	if ((GMT == 8)||(GMT == 10)){TZ_min=-30;}
	else if ((GMT == 18)||(GMT == 20)||(GMT ==22)||(GMT == 25)||(GMT == 29)){TZ_min=30;}
	else if (GMT == 23){TZ_min=45;}
	
	//Get Number of hours
	if (TimeZone == 1){
			tz1_hours= TimeZones[GMT] - local_hours;
			tz1_min=(0 + TZ_min) - local_min;
	}
	else if (TimeZone == 2){
			tz2_hours= TimeZones[GMT] - local_hours;
			tz2_min=(0 + TZ_min)- local_min;
	}
	else if (TimeZone == 3){
			tz3_hours= TimeZones[GMT] - local_hours;
			tz3_min=(0 + TZ_min) - local_min;
	}
	

}

//*****************//
// AppSync options //
//*****************//

        static AppSync sync;
        static uint8_t sync_buffer[128];



        static void sync_tuple_changed_callback(const uint32_t key,
                                        const Tuple* new_tuple,
                                        const Tuple* old_tuple,
                                        void* context) {

        
  // App Sync keeps new_tuple in sync_buffer, so we may use it directly
  switch (key) {
      case Language_KEY:
	  	intLanguage = new_tuple->value->int8;  
	  	persist_write_int(Language_KEY, intLanguage);
	  
	  	//Init the date
		getDate();
      	break;
	  
	  case LocalTime_KEY:
	  	intLocalTime = new_tuple->value->int8;  
	  	persist_write_int(LocalTime_KEY, intLocalTime);
	  
	  	CalculateTimeZone (intLocalTime,1,intTZ1);
	  	CalculateTimeZone (intLocalTime,2,intTZ2);
	  	CalculateTimeZone (intLocalTime,3,intTZ3);
	  	getTimeZones();
      	break;
	  
    case TZ1Name_KEY:
  		persist_write_string(TZ1Name_KEY, new_tuple->value->cstring);
		  //clean up the string
		  memset(&tz1_name[0], 0, sizeof(tz1_name));
	  	memcpy(&tz1_name, new_tuple->value->cstring, strlen(new_tuple->value->cstring));
	  	text_layer_set_text(WC1NAME_Layer, tz1_name);
      	break;
	  
     case TZ1Time_KEY:
	  	intTZ1 = new_tuple->value->int8;  
	  	persist_write_int(TZ1Time_KEY, intTZ1);
	  	
	  	CalculateTimeZone (intLocalTime,1,intTZ1);
	  	getTimeZones();
      	break;
	  
	 case TZ2Name_KEY:
  		persist_write_string(TZ2Name_KEY, new_tuple->value->cstring);
		  //clean up the string
		  memset(&tz2_name[0], 0, sizeof(tz2_name));
	  	memcpy(&tz2_name, new_tuple->value->cstring, strlen(new_tuple->value->cstring));
	  	text_layer_set_text(WC2NAME_Layer, tz2_name);
      	break;

     case TZ2Time_KEY:
	  	intTZ2 = new_tuple->value->int8;
	  	persist_write_int(TZ2Time_KEY, intTZ2);
	  
	  	CalculateTimeZone (intLocalTime,2,intTZ2);
	    getTimeZones();
      	break;
	  
	 case TZ3Name_KEY:
  		persist_write_string(TZ3Name_KEY, new_tuple->value->cstring);
		  //clean up the string
		  memset(&tz3_name[0], 0, sizeof(tz3_name));
	  	memcpy(&tz3_name, new_tuple->value->cstring, strlen(new_tuple->value->cstring));
	  	text_layer_set_text(WC3NAME_Layer, tz3_name);
      	break;

     case TZ3Time_KEY:
		intTZ3 = new_tuple->value->int8;
	  	persist_write_int(TZ3Time_KEY, intTZ3);
	  
	  	CalculateTimeZone (intLocalTime,3,intTZ3);
	    getTimeZones();
      	break;

	 case INVERT_COLOR_KEY:
		  color_inverted = new_tuple->value->uint8 != 0;
		  persist_write_bool(INVERT_COLOR_KEY, new_tuple->value->uint8 != 0);
	  		
	  	  //refresh the layout
	  	  InvertColors(color_inverted);
		  break;
  }
}

//************************//
// Capture the Tick event //
//************************//
void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{

	if (units_changed & MINUTE_UNIT) 
	{

			/*
			if (units_changed & DAY_UNIT)
			{	
			} // DAY CHANGES
			*/
		
		
			//Format the Local Time	
			if (clock_is_24h_style())
			{
				strftime(time_text, sizeof(time_text), "%H:%M", tick_time);
			}
			else
			{
				strftime(time_text, sizeof(time_text), "%I:%M", tick_time);
			}


  			text_layer_set_text(Time_Layer, time_text);


			//Calculate the Dual Time
			getTimeZones(); //TODO: If changing the timezones to only list hour and not minute, update this.

			//Check Battery Status
			//handle_battery(battery_state_service_peek()); //SAH -- Don't check every minute

			//Check BT Status
			//handle_bluetooth(bluetooth_connection_service_peek()); //SAH -- Don't check every minute

	} //MINUTE CHANGES

	if (units_changed & DAY_UNIT){
	 	//Update the date
	 	getDate();
	}
} //HANDLE_TICK 



//****************************//
// Initialize the application //
//****************************//

void handle_init(void)
{
	//Define Resources
    ResHandle res_d;
	ResHandle res_u;
	ResHandle res_t;
	ResHandle res_temp;
	ResHandle res_cj;
	
	
	// read saved settings
	//TODO: Set here to override language (until Japanese - 98 and Chinese - 99 are in options).
	intLanguage=persist_read_int(Language_KEY); 
	intLocalTime=persist_read_int(LocalTime_KEY);
	persist_read_string(TZ1Name_KEY, tz1_name, sizeof(tz1_name));
	intTZ1=persist_read_int(TZ1Time_KEY);
	persist_read_string(TZ2Name_KEY, tz2_name, sizeof(tz2_name));
	intTZ2=persist_read_int(TZ2Time_KEY);
	persist_read_string(TZ3Name_KEY, tz3_name, sizeof(tz3_name));
	intTZ3=persist_read_int(TZ3Time_KEY);
	color_inverted = persist_read_bool(INVERT_COLOR_KEY);

	//Create the main window
	my_window = window_create(); 
	window_stack_push(my_window, true /* Animated */);
	window_set_background_color(my_window, GColorBlack);


	//Load the custom fonts
	res_t = resource_get_handle(RESOURCE_ID_FUTURA_CONDENSED_53); // Time font
	res_d = resource_get_handle(RESOURCE_ID_FUTURA_17); // Date font
	res_u = resource_get_handle(RESOURCE_ID_FUTURA_14); // Last Update font
	res_cj = resource_get_handle(RESOURCE_ID_MINCHO_16); //Font for Chinese and Japanese day of week, etc.
	//res_temp =  resource_get_handle(RESOURCE_ID_FUTURA_36); //Temperature


    font_date = fonts_load_custom_font(res_d);
	font_update = fonts_load_custom_font(res_u);
	font_time = fonts_load_custom_font(res_t);
	font_cj = fonts_load_custom_font(res_cj);


	//LOAD THE LAYERS
		//Display the Weekday layer
		Weekday_Layer = text_layer_create(WEEKDAY_FRAME);
		text_layer_set_text_color(Weekday_Layer, GColorWhite);
		text_layer_set_background_color(Weekday_Layer, GColorClear);
		if (intLanguage==98||intLanguage==99) {
			text_layer_set_font(Weekday_Layer, font_cj);
		} else {
			text_layer_set_font(Weekday_Layer, font_date);
		}
		text_layer_set_text_alignment(Weekday_Layer, GTextAlignmentLeft);
		layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(Weekday_Layer)); 

		//Display the Batt layer
		Batt_icon_layer = bitmap_layer_create(BATT_FRAME);
  		bitmap_layer_set_bitmap(Batt_icon_layer, Batt_image);
  		layer_add_child(window_get_root_layer(my_window), bitmap_layer_get_layer(Batt_icon_layer));

		//Display the BT layer
	  	BT_icon_layer = bitmap_layer_create(BT_FRAME);
  		bitmap_layer_set_bitmap(BT_icon_layer, BT_image);
  		layer_add_child(window_get_root_layer(my_window), bitmap_layer_get_layer(BT_icon_layer));

		//Display the Time layer
		Time_Layer = text_layer_create(TIME_FRAME);
		text_layer_set_text_color(Time_Layer, GColorWhite);
		text_layer_set_background_color(Time_Layer, GColorClear);
		text_layer_set_font(Time_Layer, font_time);
		text_layer_set_text_alignment(Time_Layer, GTextAlignmentCenter);
		layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(Time_Layer)); 

		//Display the Date layer
		date_layer = text_layer_create(DATE_FRAME);
		text_layer_set_text_color(date_layer, GColorWhite);
		text_layer_set_background_color(date_layer, GColorClear);
		if (intLanguage==98||intLanguage==99)
			text_layer_set_font(date_layer, font_cj);
		else
		text_layer_set_font(date_layer, font_date);
		text_layer_set_text_alignment(date_layer, GTextAlignmentRight);
		layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(date_layer)); 

		//Display the Work Clock 1 layer
		WC1NAME_Layer = text_layer_create(WC1NAME_FRAME);
		WC1TIME_Layer = text_layer_create(WC1TIME_FRAME);
		text_layer_set_text_color(WC1NAME_Layer, GColorWhite);
		text_layer_set_text_color(WC1TIME_Layer, GColorWhite);
		text_layer_set_background_color(WC1NAME_Layer, GColorClear);
		text_layer_set_background_color(WC1TIME_Layer, GColorClear);
		text_layer_set_font(WC1NAME_Layer, font_update);
		text_layer_set_font(WC1TIME_Layer, font_update);
		text_layer_set_text_alignment(WC1NAME_Layer, GTextAlignmentLeft);
		text_layer_set_text_alignment(WC1TIME_Layer, GTextAlignmentRight);
		layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(WC1NAME_Layer)); 
		layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(WC1TIME_Layer)); 

		//Display the Work Clock 2 layer
		WC2NAME_Layer = text_layer_create(WC2NAME_FRAME);
		WC2TIME_Layer = text_layer_create(WC2TIME_FRAME);
		text_layer_set_text_color(WC2NAME_Layer, GColorWhite);
		text_layer_set_text_color(WC2TIME_Layer, GColorWhite);
		text_layer_set_background_color(WC2NAME_Layer, GColorClear);
		text_layer_set_background_color(WC2TIME_Layer, GColorClear);
		text_layer_set_font(WC2NAME_Layer, font_update);
		text_layer_set_font(WC2TIME_Layer, font_update);
		text_layer_set_text_alignment(WC2NAME_Layer, GTextAlignmentLeft);
		text_layer_set_text_alignment(WC2TIME_Layer, GTextAlignmentRight);
		layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(WC2NAME_Layer)); 
		layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(WC2TIME_Layer)); 

		//Display the Work Clock 3 layer
		WC3NAME_Layer = text_layer_create(WC3NAME_FRAME);
		WC3TIME_Layer = text_layer_create(WC3TIME_FRAME);
		text_layer_set_text_color(WC3NAME_Layer, GColorWhite);
		text_layer_set_text_color(WC3TIME_Layer, GColorWhite);
		text_layer_set_background_color(WC3NAME_Layer, GColorClear);
		text_layer_set_background_color(WC3TIME_Layer, GColorClear);
		text_layer_set_font(WC3NAME_Layer, font_update);
		text_layer_set_font(WC3TIME_Layer, font_update);
		text_layer_set_text_alignment(WC3NAME_Layer, GTextAlignmentLeft);
		text_layer_set_text_alignment(WC3TIME_Layer, GTextAlignmentRight);
		layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(WC3NAME_Layer)); 
		layer_add_child(window_get_root_layer(my_window), text_layer_get_layer(WC3TIME_Layer)); 


		//Drawn the normal/inverted based on saved settings
	    InvertColors(color_inverted);

	// Ensures time is displayed immediately (will break if NULL tick event accessed).
	  // (This is why it's a good idea to have a separate routine to do the update itself.)

		time_t now = time(NULL);
	  	struct tm *current_time = localtime(&now);
		handle_tick(current_time, MINUTE_UNIT);
		tick_timer_service_subscribe(MINUTE_UNIT, &handle_tick);

		//Enable the Battery check event
		battery_state_service_subscribe(&handle_battery);
		//Enable the Bluetooth check event
	 	bluetooth_connection_service_subscribe(&handle_bluetooth);
	
	// Setup messaging
		const int inbound_size = 128;
		const int outbound_size = 128;
		
		app_message_open(inbound_size, outbound_size);
	
	
	Tuplet initial_values[] = {
		TupletInteger(Language_KEY, intLanguage), 
		TupletInteger(LocalTime_KEY, intLocalTime), 
		MyTupletCString(TZ1Name_KEY,tz1_name),
		TupletInteger(TZ1Time_KEY, intTZ1),
		MyTupletCString(TZ2Name_KEY,tz2_name),
		TupletInteger(TZ2Time_KEY, intTZ2),
		MyTupletCString(TZ3Name_KEY,tz3_name),
		TupletInteger(TZ3Time_KEY, intTZ3),
		TupletInteger(INVERT_COLOR_KEY, color_inverted),
	}; //TUPLET INITIAL VALUES
	
	
	
	app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values,
				  ARRAY_LENGTH(initial_values), sync_tuple_changed_callback,
				  NULL, NULL);

	//Check Battery Status
	handle_battery(battery_state_service_peek());

	//Check BT Status
	handle_bluetooth(bluetooth_connection_service_peek());


} //HANDLE_INIT



//**********************//
// Kill the application //
//**********************//
void handle_deinit(void)
{
  //text_layer_destroy(text_layer);

	//Unsuscribe services
	tick_timer_service_unsubscribe();
 	battery_state_service_unsubscribe();
  	bluetooth_connection_service_unsubscribe();

	if (BT_image!=NULL) {gbitmap_destroy(BT_image);}
	if (Batt_image!=NULL){gbitmap_destroy(Batt_image);}

	//Deallocate layers
	text_layer_destroy(Time_Layer);
	text_layer_destroy(date_layer);
	text_layer_destroy(Weekday_Layer);

	//Deallocate custom fonts
	fonts_unload_custom_font(font_date);
	fonts_unload_custom_font(font_update);
	fonts_unload_custom_font(font_time);
	fonts_unload_custom_font(font_cj);

	//Deallocate the main window
  	window_destroy(my_window);

} //HANDLE_DEINIT


//*************//
// ENTRY POINT //
//*************//
int main(void) 
{	
	handle_init();
	app_event_loop();
	handle_deinit();
}
