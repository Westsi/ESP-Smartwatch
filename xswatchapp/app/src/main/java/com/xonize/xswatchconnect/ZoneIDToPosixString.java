package com.xonize.xswatchconnect;

import java.util.HashMap;


/*
* STRINGS FROM THIS GUY I LOVE YOU
* https://gist.github.com/tivaliy/a5ef9c7ccb4d57bdc248f0855c5eb1ff
* */
public class ZoneIDToPosixString {
    public static HashMap<String, String> ids = new HashMap<>();
    public static void setupmap() {
        ids.put("Africa/Abidjan", "GMT0");
        ids.put("Africa/Accra", "GMT0");
        ids.put("Africa/Addis_Ababa", "EAT-3");
        ids.put("Africa/Algiers", "CET-1");
        ids.put("Africa/Asmara", "EAT-3");
        ids.put("Africa/Asmera", "EAT-3");
        ids.put("Africa/Bamako", "GMT0");
        ids.put("Africa/Bangui", "WAT-1");
        ids.put("Africa/Banjul", "GMT0");
        ids.put("Africa/Bissau", "GMT0");
        ids.put("Africa/Blantyre", "CAT-2");
        ids.put("Africa/Brazzaville", "WAT-1");
        ids.put("Africa/Bujumbura", "CAT-2");
        ids.put("Africa/Cairo", "EEST");
        ids.put("Africa/Casablanca", "WET0");
        ids.put("Africa/Ceuta", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Africa/Conakry", "GMT0");
        ids.put("Africa/Dakar", "GMT0");
        ids.put("Africa/Dar_es_Salaam", "EAT-3");
        ids.put("Africa/Djibouti", "EAT-3");
        ids.put("Africa/Douala", "WAT-1");
        ids.put("Africa/El_Aaiun", "WET0");
        ids.put("Africa/Freetown", "GMT0");
        ids.put("Africa/Gaborone", "CAT-2");
        ids.put("Africa/Harare", "CAT-2");
        ids.put("Africa/Johannesburg", "SAST-2");
        ids.put("Africa/Kampala", "EAT-3");
        ids.put("Africa/Khartoum", "EAT-3");
        ids.put("Africa/Kigali", "CAT-2");
        ids.put("Africa/Kinshasa", "WAT-1");
        ids.put("Africa/Lagos", "WAT-1");
        ids.put("Africa/Libreville", "WAT-1");
        ids.put("Africa/Lome", "GMT0");
        ids.put("Africa/Luanda", "WAT-1");
        ids.put("Africa/Lubumbashi", "CAT-2");
        ids.put("Africa/Lusaka", "CAT-2");
        ids.put("Africa/Malabo", "WAT-1");
        ids.put("Africa/Maputo", "CAT-2");
        ids.put("Africa/Maseru", "SAST-2");
        ids.put("Africa/Mbabane", "SAST-2");
        ids.put("Africa/Mogadishu", "EAT-3");
        ids.put("Africa/Monrovia", "GMT0");
        ids.put("Africa/Nairobi", "EAT-3");
        ids.put("Africa/Ndjamena", "WAT-1");
        ids.put("Africa/Niamey", "WAT-1");
        ids.put("Africa/Nouakchott", "GMT0");
        ids.put("Africa/Ouagadougou", "GMT0");
        ids.put("Africa/Porto-Novo", "WAT-1");
        ids.put("Africa/Sao_Tome", "GMT0");
        ids.put("Africa/Timbuktu", "GMT0");
        ids.put("Africa/Tripoli", "EET-2");
        ids.put("Africa/Tunis", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Africa/Windhoek", "WAT-1WAST,M9.1.0,M4.1.0");
        ids.put("America/Adak", "HAST10HADT,M3.2.0,M11.1.0");
        ids.put("America/Anchorage", "AKST9AKDT,M3.2.0,M11.1.0");
        ids.put("America/Anguilla", "AST4");
        ids.put("America/Antigua", "AST4");
        ids.put("America/Araguaina", "BRT3");
        ids.put("America/Argentina/Buenos_Aires", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Argentina/Catamarca", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Argentina/ComodRivadavia", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Argentina/Cordoba", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Argentina/Jujuy", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Argentina/La_Rioja", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Argentina/Mendoza", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Argentina/Rio_Gallegos", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Argentina/San_Juan", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Argentina/San_Luis", "ART3");
        ids.put("America/Argentina/Tucuman", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Argentina/Ushuaia", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Aruba", "AST4");
        ids.put("America/Asuncion", "PYT4PYST,M10.3.0/0,M3.2.0/0");
        ids.put("America/Atikokan", "EST5");
        ids.put("America/Atka", "HAST10HADT,M3.2.0,M11.1.0");
        ids.put("America/Bahia", "BRT3");
        ids.put("America/Barbados", "AST4");
        ids.put("America/Belem", "BRT3");
        ids.put("America/Belize", "CST6");
        ids.put("America/Blanc-Sablon", "AST4");
        ids.put("America/Boa_Vista", "AMT4");
        ids.put("America/Bogota", "COT5");
        ids.put("America/Boise", "MST7MDT,M3.2.0,M11.1.0");
        ids.put("America/Buenos_Aires", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Cambridge_Bay", "MST7MDT,M3.2.0,M11.1.0");
        ids.put("America/Campo_Grande", "AMT4AMST,M10.2.0/0,M2.3.0/0");
        ids.put("America/Cancun", "CST6CDT,M4.1.0,M10.5.0");
        ids.put("America/Caracas", "VET4:30");
        ids.put("America/Catamarca", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Cayenne", "GFT3");
        ids.put("America/Cayman", "EST5");
        ids.put("America/Chicago", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("America/Chihuahua", "MST7MDT,M4.1.0,M10.5.0");
        ids.put("America/Coral_Harbour", "EST5");
        ids.put("America/Cordoba", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Costa_Rica", "CST6");
        ids.put("America/Cuiaba", "AMT4AMST,M10.2.0/0,M2.3.0/0");
        ids.put("America/Curacao", "AST4");
        ids.put("America/Danmarkshavn", "GMT0");
        ids.put("America/Dawson", "PST8PDT,M3.2.0,M11.1.0");
        ids.put("America/Dawson_Creek", "MST7");
        ids.put("America/Denver", "MST7MDT,M3.2.0,M11.1.0");
        ids.put("America/Detroit", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Dominica", "AST4");
        ids.put("America/Edmonton", "MST7MDT,M3.2.0,M11.1.0");
        ids.put("America/Eirunepe", "ACT5");
        ids.put("America/El_Salvador", "CST6");
        ids.put("America/Ensenada", "PST8PDT,M4.1.0,M10.5.0");
        ids.put("America/Fortaleza", "BRT3");
        ids.put("America/Fort_Wayne", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Glace_Bay", "AST4ADT,M3.2.0,M11.1.0");
        ids.put("America/Godthab", "WGST");
        ids.put("America/Goose_Bay", "AST4ADT,M3.2.0/0:01,M11.1.0/0:01");
        ids.put("America/Grand_Turk", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Grenada", "AST4");
        ids.put("America/Guadeloupe", "AST4");
        ids.put("America/Guatemala", "CST6");
        ids.put("America/Guayaquil", "ECT5");
        ids.put("America/Guyana", "GYT4");
        ids.put("America/Halifax", "AST4ADT,M3.2.0,M11.1.0");
        ids.put("America/Havana", "CST5CDT,M3.3.0/0,M10.5.0/1");
        ids.put("America/Hermosillo", "MST7");
        ids.put("America/Indiana/Indianapolis", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Indiana/Knox", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("America/Indiana/Marengo", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Indiana/Petersburg", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Indianapolis", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Indiana/Tell_City", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("America/Indiana/Vevay", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Indiana/Vincennes", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Indiana/Winamac", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Inuvik", "MST7MDT,M3.2.0,M11.1.0");
        ids.put("America/Iqaluit", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Jamaica", "EST5");
        ids.put("America/Jujuy", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Juneau", "AKST9AKDT,M3.2.0,M11.1.0");
        ids.put("America/Kentucky/Louisville", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Kentucky/Monticello", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Knox_IN", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("America/La_Paz", "BOT4");
        ids.put("America/Lima", "PET5");
        ids.put("America/Los_Angeles", "PST8PDT,M3.2.0,M11.1.0");
        ids.put("America/Louisville", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Maceio", "BRT3");
        ids.put("America/Managua", "CST6");
        ids.put("America/Manaus", "AMT4");
        ids.put("America/Marigot", "AST4");
        ids.put("America/Martinique", "AST4");
        ids.put("America/Mazatlan", "MST7MDT,M4.1.0,M10.5.0");
        ids.put("America/Mendoza", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Menominee", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("America/Merida", "CST6CDT,M4.1.0,M10.5.0");
        ids.put("America/Mexico_City", "CST6CDT,M4.1.0,M10.5.0");
        ids.put("America/Miquelon", "PMST3PMDT,M3.2.0,M11.1.0");
        ids.put("America/Moncton", "AST4ADT,M3.2.0,M11.1.0");
        ids.put("America/Monterrey", "CST6CDT,M4.1.0,M10.5.0");
        ids.put("America/Montevideo", "UYT3UYST,M10.1.0,M3.2.0");
        ids.put("America/Montreal", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Montserrat", "AST4");
        ids.put("America/Nassau", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/New_York", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Nipigon", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Nome", "AKST9AKDT,M3.2.0,M11.1.0");
        ids.put("America/Noronha", "FNT2");
        ids.put("America/North_Dakota/Center", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("America/North_Dakota/New_Salem", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("America/Panama", "EST5");
        ids.put("America/Pangnirtung", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Paramaribo", "SRT3");
        ids.put("America/Phoenix", "MST7");
        ids.put("America/Port-au-Prince", "EST5");
        ids.put("America/Porto_Acre", "ACT5");
        ids.put("America/Port_of_Spain", "AST4");
        ids.put("America/Porto_Velho", "AMT4");
        ids.put("America/Puerto_Rico", "AST4");
        ids.put("America/Rainy_River", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("America/Rankin_Inlet", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("America/Recife", "BRT3");
        ids.put("America/Regina", "CST6");
        ids.put("America/Resolute", "EST5");
        ids.put("America/Rio_Branco", "ACT5");
        ids.put("America/Rosario", "ART3ARST,M10.1.0/0,M3.3.0/0");
        ids.put("America/Santiago", "CLST");
        ids.put("America/Santo_Domingo", "AST4");
        ids.put("America/Sao_Paulo", "BRT3BRST,M10.2.0/0,M2.3.0/0");
        ids.put("America/Scoresbysund", "EGT1EGST,M3.5.0/0,M10.5.0/1");
        ids.put("America/Shiprock", "MST7MDT,M3.2.0,M11.1.0");
        ids.put("America/St_Barthelemy", "AST4");
        ids.put("America/St_Johns", "NST3:30NDT,M3.2.0/0:01,M11.1.0/0:01");
        ids.put("America/St_Kitts", "AST4");
        ids.put("America/St_Lucia", "AST4");
        ids.put("America/St_Thomas", "AST4");
        ids.put("America/St_Vincent", "AST4");
        ids.put("America/Swift_Current", "CST6");
        ids.put("America/Tegucigalpa", "CST6");
        ids.put("America/Thule", "AST4ADT,M3.2.0,M11.1.0");
        ids.put("America/Thunder_Bay", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Tijuana", "PST8PDT,M4.1.0,M10.5.0");
        ids.put("America/Toronto", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("America/Tortola", "AST4");
        ids.put("America/Vancouver", "PST8PDT,M3.2.0,M11.1.0");
        ids.put("America/Virgin", "AST4");
        ids.put("America/Whitehorse", "PST8PDT,M3.2.0,M11.1.0");
        ids.put("America/Winnipeg", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("America/Yakutat", "AKST9AKDT,M3.2.0,M11.1.0");
        ids.put("America/Yellowknife", "MST7MDT,M3.2.0,M11.1.0");
        ids.put("Antarctica/Casey", "WST-8");
        ids.put("Antarctica/Davis", "DAVT-7");
        ids.put("Antarctica/DumontDUrville", "DDUT-10");
        ids.put("Antarctica/Mawson", "MAWT-6");
        ids.put("Antarctica/McMurdo", "NZST-12NZDT,M9.5.0,M4.1.0/3");
        ids.put("Antarctica/Palmer", "CLST");
        ids.put("Antarctica/Rothera", "ROTT3");
        ids.put("Antarctica/South_Pole", "NZST-12NZDT,M9.5.0,M4.1.0/3");
        ids.put("Antarctica/Syowa", "SYOT-3");
        ids.put("Antarctica/Vostok", "VOST-6");
        ids.put("Arctic/Longyearbyen", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Aden", "AST-3");
        ids.put("Asia/Almaty", "ALMT-6");
        ids.put("Asia/Amman", "EET-2EEST,M3.5.4/0,M10.5.5/1");
        ids.put("Asia/Anadyr", "ANAT-12ANAST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Aqtau", "AQTT-5");
        ids.put("Asia/Aqtobe", "AQTT-5");
        ids.put("Asia/Ashgabat", "TMT-5");
        ids.put("Asia/Ashkhabad", "TMT-5");
        ids.put("Asia/Baghdad", "AST-3");
        ids.put("Asia/Bahrain", "AST-3");
        ids.put("Asia/Baku", "AZT-4AZST,M3.5.0/4,M10.5.0/5");
        ids.put("Asia/Bangkok", "ICT-7");
        ids.put("Asia/Beirut", "EET-2EEST,M3.5.0/0,M10.5.0/0");
        ids.put("Asia/Bishkek", "KGT-6");
        ids.put("Asia/Brunei", "BNT-8");
        ids.put("Asia/Calcutta", "IST-5:30");
        ids.put("Asia/Choibalsan", "CHOT-9");
        ids.put("Asia/Chongqing", "CST-8");
        ids.put("Asia/Chungking", "CST-8");
        ids.put("Asia/Colombo", "IST-5:30");
        ids.put("Asia/Dacca", "BDT-6");
        ids.put("Asia/Damascus", "EET-2EEST,M4.1.5/0,J274/0");
        ids.put("Asia/Dhaka", "BDT-6");
        ids.put("Asia/Dili", "TLT-9");
        ids.put("Asia/Dubai", "GST-4");
        ids.put("Asia/Dushanbe", "TJT-5");
        ids.put("Asia/Gaza", "EET-2EEST,J91/0,M9.2.4");
        ids.put("Asia/Harbin", "CST-8");
        ids.put("Asia/Ho_Chi_Minh", "ICT-7");
        ids.put("Asia/Hong_Kong", "HKT-8");
        ids.put("Asia/Hovd", "HOVT-7");
        ids.put("Asia/Irkutsk", "IRKT-8IRKST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Istanbul", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Asia/Jakarta", "WIT-7");
        ids.put("Asia/Jayapura", "EIT-9");
        ids.put("Asia/Jerusalem", "IDDT");
        ids.put("Asia/Kabul", "AFT-4:30");
        ids.put("Asia/Kamchatka", "PETT-12PETST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Karachi", "PKT-5");
        ids.put("Asia/Kashgar", "CST-8");
        ids.put("Asia/Katmandu", "NPT-5:45");
        ids.put("Asia/Kolkata", "IST-5:30");
        ids.put("Asia/Krasnoyarsk", "KRAT-7KRAST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Kuala_Lumpur", "MYT-8");
        ids.put("Asia/Kuching", "MYT-8");
        ids.put("Asia/Kuwait", "AST-3");
        ids.put("Asia/Macao", "CST-8");
        ids.put("Asia/Macau", "CST-8");
        ids.put("Asia/Magadan", "MAGT-11MAGST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Makassar", "CIT-8");
        ids.put("Asia/Manila", "PHT-8");
        ids.put("Asia/Muscat", "GST-4");
        ids.put("Asia/Nicosia", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Asia/Novosibirsk", "NOVT-6NOVST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Omsk", "OMST-6OMSST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Oral", "ORAT-5");
        ids.put("Asia/Phnom_Penh", "ICT-7");
        ids.put("Asia/Pontianak", "WIT-7");
        ids.put("Asia/Pyongyang", "KST-9");
        ids.put("Asia/Qatar", "AST-3");
        ids.put("Asia/Qyzylorda", "QYZT-6");
        ids.put("Asia/Rangoon", "MMT-6:30");
        ids.put("Asia/Riyadh", "AST-3");
        ids.put("Asia/Riyadh87", "zzz-3:07:04");
        ids.put("Asia/Riyadh88", "zzz-3:07:04");
        ids.put("Asia/Riyadh89", "zzz-3:07:04");
        ids.put("Asia/Saigon", "ICT-7");
        ids.put("Asia/Sakhalin", "SAKT-10SAKST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Samarkand", "UZT-5");
        ids.put("Asia/Seoul", "KST-9");
        ids.put("Asia/Shanghai", "CST-8");
        ids.put("Asia/Singapore", "SGT-8");
        ids.put("Asia/Taipei", "CST-8");
        ids.put("Asia/Tashkent", "UZT-5");
        ids.put("Asia/Tbilisi", "GET-4");
        ids.put("Asia/Tehran", "IRDT");
        ids.put("Asia/Tel_Aviv", "IDDT");
        ids.put("Asia/Thimbu", "BTT-6");
        ids.put("Asia/Thimphu", "BTT-6");
        ids.put("Asia/Tokyo", "JST-9");
        ids.put("Asia/Ujung_Pandang", "CIT-8");
        ids.put("Asia/Ulaanbaatar", "ULAT-8");
        ids.put("Asia/Ulan_Bator", "ULAT-8");
        ids.put("Asia/Urumqi", "CST-8");
        ids.put("Asia/Vientiane", "ICT-7");
        ids.put("Asia/Vladivostok", "VLAT-10VLAST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Yakutsk", "YAKT-9YAKST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Yekaterinburg", "YEKT-5YEKST,M3.5.0,M10.5.0/3");
        ids.put("Asia/Yerevan", "AMT-4AMST,M3.5.0,M10.5.0/3");
        ids.put("Atlantic/Azores", "AZOT1AZOST,M3.5.0/0,M10.5.0/1");
        ids.put("Atlantic/Bermuda", "AST4ADT,M3.2.0,M11.1.0");
        ids.put("Atlantic/Canary", "WET0WEST,M3.5.0/1,M10.5.0");
        ids.put("Atlantic/Cape_Verde", "CVT1");
        ids.put("Atlantic/Faeroe", "WET0WEST,M3.5.0/1,M10.5.0");
        ids.put("Atlantic/Faroe", "WET0WEST,M3.5.0/1,M10.5.0");
        ids.put("Atlantic/Jan_Mayen", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Atlantic/Madeira", "WET0WEST,M3.5.0/1,M10.5.0");
        ids.put("Atlantic/Reykjavik", "GMT0");
        ids.put("Atlantic/South_Georgia", "GST2");
        ids.put("Atlantic/Stanley", "FKT4FKST,M9.1.0,M4.3.0");
        ids.put("Atlantic/St_Helena", "GMT0");
        ids.put("Australia/ACT", "EST-10EST,M10.1.0,M4.1.0/3");
        ids.put("Australia/Adelaide", "CST-9:30CST,M10.1.0,M4.1.0/3");
        ids.put("Australia/Brisbane", "EST-10");
        ids.put("Australia/Broken_Hill", "CST-9:30CST,M10.1.0,M4.1.0/3");
        ids.put("Australia/Canberra", "EST-10EST,M10.1.0,M4.1.0/3");
        ids.put("Australia/Currie", "EST-10EST,M10.1.0,M4.1.0/3");
        ids.put("Australia/Darwin", "CST-9:30");
        ids.put("Australia/Eucla", "CWST-8:45");
        ids.put("Australia/Hobart", "EST-10EST,M10.1.0,M4.1.0/3");
        ids.put("Australia/LHI", "LHST-10:30LHST-11,M10.1.0,M4.1.0");
        ids.put("Australia/Lindeman", "EST-10");
        ids.put("Australia/Lord_Howe", "LHST-10:30LHST-11,M10.1.0,M4.1.0");
        ids.put("Australia/Melbourne", "EST-10EST,M10.1.0,M4.1.0/3");
        ids.put("Australia/North", "CST-9:30");
        ids.put("Australia/NSW", "EST-10EST,M10.1.0,M4.1.0/3");
        ids.put("Australia/Perth", "WST-8");
        ids.put("Australia/Queensland", "EST-10");
        ids.put("Australia/South", "CST-9:30CST,M10.1.0,M4.1.0/3");
        ids.put("Australia/Sydney", "EST-10EST,M10.1.0,M4.1.0/3");
        ids.put("Australia/Tasmania", "EST-10EST,M10.1.0,M4.1.0/3");
        ids.put("Australia/Victoria", "EST-10EST,M10.1.0,M4.1.0/3");
        ids.put("Australia/West", "WST-8");
        ids.put("Australia/Yancowinna", "CST-9:30CST,M10.1.0,M4.1.0/3");
        ids.put("Brazil/Acre", "ACT5");
        ids.put("Brazil/DeNoronha", "FNT2");
        ids.put("Brazil/East", "BRT3BRST,M10.2.0/0,M2.3.0/0");
        ids.put("Brazil/West", "AMT4");
        ids.put("Canada/Atlantic", "AST4ADT,M3.2.0,M11.1.0");
        ids.put("Canada/Central", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("Canada/Eastern", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("Canada/East-Saskatchewan", "CST6");
        ids.put("Canada/Mountain", "MST7MDT,M3.2.0,M11.1.0");
        ids.put("Canada/Newfoundland", "NST3:30NDT,M3.2.0/0:01,M11.1.0/0:01");
        ids.put("Canada/Pacific", "PST8PDT,M3.2.0,M11.1.0");
        ids.put("Canada/Saskatchewan", "CST6");
        ids.put("Canada/Yukon", "PST8PDT,M3.2.0,M11.1.0");
        ids.put("Chile/Continental", "CLST");
        ids.put("Chile/EasterIsland", "EASST");
        ids.put("Etc/GMT", "GMT0");
        ids.put("Etc/GMT0", "GMT0");
        ids.put("Etc/GMT-0", "GMT0");
        ids.put("Etc/GMT+0", "GMT0");
        ids.put("Etc/GMT-1", "GMT-1");
        ids.put("Etc/GMT+1", "GMT+1");
        ids.put("Etc/GMT-10", "GMT-10");
        ids.put("Etc/GMT+10", "GMT+10");
        ids.put("Etc/GMT-11", "GMT-11");
        ids.put("Etc/GMT+11", "GMT+11");
        ids.put("Etc/GMT-12", "GMT-12");
        ids.put("Etc/GMT+12", "GMT+12");
        ids.put("Etc/GMT-13", "GMT-13");
        ids.put("Etc/GMT-14", "GMT-14");
        ids.put("Etc/GMT-2", "GMT-2");
        ids.put("Etc/GMT+2", "GMT+2");
        ids.put("Etc/GMT-3", "GMT-3");
        ids.put("Etc/GMT+3", "GMT+3");
        ids.put("Etc/GMT-4", "GMT-4");
        ids.put("Etc/GMT+4", "GMT+4");
        ids.put("Etc/GMT-5", "GMT-5");
        ids.put("Etc/GMT+5", "GMT+5");
        ids.put("Etc/GMT-6", "GMT-6");
        ids.put("Etc/GMT+6", "GMT+6");
        ids.put("Etc/GMT-7", "GMT-7");
        ids.put("Etc/GMT+7", "GMT+7");
        ids.put("Etc/GMT-8", "GMT-8");
        ids.put("Etc/GMT+8", "GMT+8");
        ids.put("Etc/GMT-9", "GMT-9");
        ids.put("Etc/GMT+9", "GMT+9");
        ids.put("Etc/Greenwich", "GMT0");
        ids.put("Etc/UCT", "UCT0");
        ids.put("Etc/Universal", "UTC0");
        ids.put("Etc/UTC", "UTC0");
        ids.put("Etc/Zulu", "UTC0");
        ids.put("Europe/Amsterdam", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Andorra", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Athens", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Belfast", "GMT0BST,M3.5.0/1,M10.5.0");
        ids.put("Europe/Belgrade", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Berlin", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Bratislava", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Brussels", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Bucharest", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Budapest", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Chisinau", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Copenhagen", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Dublin", "GMT0IST,M3.5.0/1,M10.5.0");
        ids.put("Europe/Gibraltar", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Guernsey", "GMT0BST,M3.5.0/1,M10.5.0");
        ids.put("Europe/Helsinki", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Isle_of_Man", "GMT0BST,M3.5.0/1,M10.5.0");
        ids.put("Europe/Istanbul", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Jersey", "GMT0BST,M3.5.0/1,M10.5.0");
        ids.put("Europe/Kaliningrad", "EET-2EEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Kiev", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Lisbon", "WET0WEST,M3.5.0/1,M10.5.0");
        ids.put("Europe/Ljubljana", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/London", "GMT0BST,M3.5.0/1,M10.5.0");
        ids.put("Europe/Luxembourg", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Madrid", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Malta", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Mariehamn", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Minsk", "EET-2EEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Monaco", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Moscow", "MSK-3MSD,M3.5.0,M10.5.0/3");
        ids.put("Europe/Nicosia", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Oslo", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Paris", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Podgorica", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Prague", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Riga", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Rome", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Samara", "SAMT-4SAMST,M3.5.0,M10.5.0/3");
        ids.put("Europe/San_Marino", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Sarajevo", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Simferopol", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Skopje", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Sofia", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Stockholm", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Tallinn", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Tirane", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Tiraspol", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Uzhgorod", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Vaduz", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Vatican", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Vienna", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Vilnius", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Volgograd", "VOLT-3VOLST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Warsaw", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Zagreb", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Europe/Zaporozhye", "EET-2EEST,M3.5.0/3,M10.5.0/4");
        ids.put("Europe/Zurich", "CET-1CEST,M3.5.0,M10.5.0/3");
        ids.put("Indian/Antananarivo", "EAT-3");
        ids.put("Indian/Chagos", "IOT-6");
        ids.put("Indian/Christmas", "CXT-7");
        ids.put("Indian/Cocos", "CCT-6:30");
        ids.put("Indian/Comoro", "EAT-3");
        ids.put("Indian/Kerguelen", "TFT-5");
        ids.put("Indian/Mahe", "SCT-4");
        ids.put("Indian/Maldives", "MVT-5");
        ids.put("Indian/Mauritius", "MUT-4");
        ids.put("Indian/Mayotte", "EAT-3");
        ids.put("Indian/Reunion", "RET-4");
        ids.put("Mexico/BajaNorte", "PST8PDT,M4.1.0,M10.5.0");
        ids.put("Mexico/BajaSur", "MST7MDT,M4.1.0,M10.5.0");
        ids.put("Mexico/General", "CST6CDT,M4.1.0,M10.5.0");
        ids.put("Mideast/Riyadh87", "zzz-3:07:04");
        ids.put("Mideast/Riyadh88", "zzz-3:07:04");
        ids.put("Mideast/Riyadh89", "zzz-3:07:04");
        ids.put("Pacific/Apia", "WST11");
        ids.put("Pacific/Auckland", "NZST-12NZDT,M9.5.0,M4.1.0/3");
        ids.put("Pacific/Chatham", "CHAST-12:45CHADT,M9.5.0/2:45,M4.1.0/3:45");
        ids.put("Pacific/Easter", "EASST");
        ids.put("Pacific/Efate", "VUT-11");
        ids.put("Pacific/Enderbury", "PHOT-13");
        ids.put("Pacific/Fakaofo", "TKT10");
        ids.put("Pacific/Fiji", "FJT-12");
        ids.put("Pacific/Funafuti", "TVT-12");
        ids.put("Pacific/Galapagos", "GALT6");
        ids.put("Pacific/Gambier", "GAMT9");
        ids.put("Pacific/Guadalcanal", "SBT-11");
        ids.put("Pacific/Guam", "ChST-10");
        ids.put("Pacific/Honolulu", "HST10");
        ids.put("Pacific/Johnston", "HST10");
        ids.put("Pacific/Kiritimati", "LINT-14");
        ids.put("Pacific/Kosrae", "KOST-11");
        ids.put("Pacific/Kwajalein", "MHT-12");
        ids.put("Pacific/Majuro", "MHT-12");
        ids.put("Pacific/Marquesas", "MART9:30");
        ids.put("Pacific/Midway", "SST11");
        ids.put("Pacific/Nauru", "NRT-12");
        ids.put("Pacific/Niue", "NUT11");
        ids.put("Pacific/Norfolk", "NFT-11:30");
        ids.put("Pacific/Noumea", "NCT-11");
        ids.put("Pacific/Pago_Pago", "SST11");
        ids.put("Pacific/Palau", "PWT-9");
        ids.put("Pacific/Pitcairn", "PST8");
        ids.put("Pacific/Ponape", "PONT-11");
        ids.put("Pacific/Port_Moresby", "PGT-10");
        ids.put("Pacific/Rarotonga", "CKT10");
        ids.put("Pacific/Saipan", "ChST-10");
        ids.put("Pacific/Samoa", "SST11");
        ids.put("Pacific/Tahiti", "TAHT10");
        ids.put("Pacific/Tarawa", "GILT-12");
        ids.put("Pacific/Tongatapu", "TOT-13");
        ids.put("Pacific/Truk", "TRUT-10");
        ids.put("Pacific/Wake", "WAKT-12");
        ids.put("Pacific/Wallis", "WFT-12");
        ids.put("Pacific/Yap", "TRUT-10");
        ids.put("SystemV/AST4", "AST4");
        ids.put("SystemV/AST4ADT", "AST4ADT,M3.2.0,M11.1.0");
        ids.put("SystemV/CST6", "CST6");
        ids.put("SystemV/CST6CDT", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("SystemV/EST5", "EST5");
        ids.put("SystemV/EST5EDT", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("SystemV/HST10", "HST10");
        ids.put("SystemV/MST7", "MST7");
        ids.put("SystemV/MST7MDT", "MST7MDT,M3.2.0,M11.1.0");
        ids.put("SystemV/PST8", "PST8");
        ids.put("SystemV/PST8PDT", "PST8PDT,M3.2.0,M11.1.0");
        ids.put("SystemV/YST9", "GAMT9");
        ids.put("SystemV/YST9YDT", "AKST9AKDT,M3.2.0,M11.1.0");
        ids.put("US/Alaska", "AKST9AKDT,M3.2.0,M11.1.0");
        ids.put("US/Aleutian", "HAST10HADT,M3.2.0,M11.1.0");
        ids.put("US/Arizona", "MST7");
        ids.put("US/Central", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("US/Eastern", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("US/East-Indiana", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("US/Hawaii", "HST10");
        ids.put("US/Indiana-Starke", "CST6CDT,M3.2.0,M11.1.0");
        ids.put("US/Michigan", "EST5EDT,M3.2.0,M11.1.0");
        ids.put("US/Mountain", "MST7MDT,M3.2.0,M11.1.0");
        ids.put("US/Pacific", "PST8PDT,M3.2.0,M11.1.0");
        ids.put("US/Samoa", "SST11");
    }
}