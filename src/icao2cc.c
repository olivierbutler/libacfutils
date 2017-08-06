/*
 * CDDL HEADER START
 *
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 *
 * CDDL HEADER END
*/
/*
 * Copyright 2017 Saso Kiselkov. All rights reserved.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <acfutils/icao2cc.h>

typedef struct {
	const char *icao;
	const char *cc;
	const char *lang;
} icao2cc_t;

/*
 * Although we'd love to have this table be uniquely keyed by ICAO code,
 * unfortunately not every place has its own unique ICAO prefix. However,
 * they're usually small enough that we don't need to worry too much.
 * Not all ICAO codes are strictly prefix-based. So to avoid matching a
 * more general (shorter) code before a more specific (longer) one, we lay
 * this table out to place more specific codes ahead of the more general
 * ones.
 */
static icao2cc_t icao2cc_table[] = {
    { .icao = "AG", .cc = "SB", .lang = "XX" },	/* Solomon Islands */
    { .icao = "AN", .cc = "NR", .lang = "XX" },	/* Nauru */
    { .icao = "AY", .cc = "PG", .lang = "XX" },	/* Papua New Guinea */
    { .icao = "BG", .cc = "GL", .lang = "kl" },	/* Greenland */
    { .icao = "BI", .cc = "IS", .lang = "is" },	/* Iceland */
    { .icao = "BK", .cc = "XK", .lang = "sq" },	/* Kosovo */
    { .icao = "C", .cc = "CA", .lang = "en" },	/* Canada */
    { .icao = "DA", .cc = "DZ", .lang = "ar" },	/* Algeria */
    { .icao = "DB", .cc = "BJ", .lang = "fr" },	/* Benin */
    { .icao = "DF", .cc = "BF", .lang = "fr" },	/* Burkina Faso */
    { .icao = "DG", .cc = "GH", .lang = "en" },	/* Ghana */
    { .icao = "DI", .cc = "CI", .lang = "fr" },	/* Ivory Coast */
    { .icao = "DN", .cc = "NG", .lang = "en" },	/* Nigeria */
    { .icao = "DR", .cc = "NE", .lang = "XX" },	/* Niger */
    { .icao = "DT", .cc = "TN", .lang = "ar" },	/* Tunisia */
    { .icao = "DX", .cc = "TG", .lang = "XX" },	/* Togo */
    { .icao = "EB", .cc = "BE", .lang = "fr" },	/* Belgium */
    { .icao = "ED", .cc = "DE", .lang = "de" },	/* Germany */
    { .icao = "EE", .cc = "EE", .lang = "et" },	/* Estonia */
    { .icao = "EF", .cc = "FI", .lang = "sv" },	/* Finland */
    { .icao = "EG", .cc = "GB", .lang = "en" },	/* United Kingdom */
    { .icao = "EG", .cc = "GS", .lang = "XX" },	/* South Georgia and the */
						/* South Sandwich Islands */
    { .icao = "EH", .cc = "NL", .lang = "nl" },	/* Netherlands */
    { .icao = "EI", .cc = "IE", .lang = "en" },	/* Ireland */
    { .icao = "EK", .cc = "DK", .lang = "da" },	/* Denmark */
    { .icao = "EL", .cc = "LU", .lang = "de" },	/* Luxembourg */
    { .icao = "EN", .cc = "MH", .lang = "en" },	/* Marshall Islands */
    { .icao = "EN", .cc = "NO", .lang = "nn" },	/* Norway */
    { .icao = "EP", .cc = "PL", .lang = "pl" },	/* Poland */
    { .icao = "ES", .cc = "SE", .lang = "sv" },	/* Sweden */
    { .icao = "ET", .cc = "DE", .lang = "de" },	/* Germany */
    { .icao = "EV", .cc = "LV", .lang = "lv" },	/* Latvia */
    { .icao = "EY", .cc = "LT", .lang = "lt" },	/* Lithuania */
    { .icao = "FA", .cc = "ZA", .lang = "en" },	/* South Africa */
    { .icao = "FB", .cc = "BW", .lang = "en" },	/* Botswana */
    { .icao = "FC", .cc = "CG", .lang = "fr" },	/* Republic of the Congo */
    { .icao = "FD", .cc = "SZ", .lang = "en" },	/* Swaziland */
    { .icao = "FE", .cc = "CF", .lang = "fr" },	/* Central African Republic */
    { .icao = "FG", .cc = "GQ", .lang = "es" },	/* Equatorial Guinea */
    { .icao = "FH", .cc = "SH", .lang = "en" },	/* Saint Helena Ascension */
						/* and Tristan da Cunha */
    { .icao = "FI", .cc = "MU", .lang = "XX" },	/* Mauritius */
    { .icao = "FJ", .cc = "IO", .lang = "en" },	/* British Indian Ocean */
						/* Territory */
    { .icao = "FK", .cc = "CM", .lang = "fr" },	/* Cameroon */
    { .icao = "FL", .cc = "ZM", .lang = "XX" },	/* Zambia */
    { .icao = "FMC", .cc = "KM", .lang = "XX" },/* Comoros */
    { .icao = "FME", .cc = "RE", .lang = "XX" },/* Réunion */
    { .icao = "FMM", .cc = "MG", .lang = "fr" },/* Madagascar */
    { .icao = "FMN", .cc = "MG", .lang = "fr" },/* Madagascar */
    { .icao = "FMS", .cc = "MG", .lang = "fr" },/* Madagascar */
    { .icao = "FM", .cc = "YT", .lang = "XX" },	/* Mayotte */
    { .icao = "FN", .cc = "AO", .lang = "XX" },	/* Angola */
    { .icao = "FO", .cc = "GA", .lang = "XX" },	/* Gabon */
    { .icao = "FP", .cc = "ST", .lang = "XX" },	/* São Tomé and Príncipe */
    { .icao = "FQ", .cc = "MZ", .lang = "pt" },	/* Mozambique */
    { .icao = "FS", .cc = "SC", .lang = "XX" },	/* Seychelles */
    { .icao = "FT", .cc = "TD", .lang = "XX" },	/* Chad */
    { .icao = "FV", .cc = "ZW", .lang = "en" },	/* Zimbabwe */
    { .icao = "FW", .cc = "MW", .lang = "XX" },	/* Malawi */
    { .icao = "FX", .cc = "LS", .lang = "XX" },	/* Lesotho */
    { .icao = "FY", .cc = "NA", .lang = "XX" },	/* Namibia */
    { .icao = "FZ", .cc = "CD", .lang = "XX" },	/* Democratic Republic of */
						/* the Congo */
    { .icao = "GA", .cc = "ML", .lang = "XX" },	/* Mali */
    { .icao = "GB", .cc = "GM", .lang = "XX" },	/* Gambia */
    { .icao = "GC", .cc = "ES", .lang = "es" },	/* Spain */
    { .icao = "GE", .cc = "ES", .lang = "es" },	/* Spain */
    { .icao = "GF", .cc = "SL", .lang = "XX" },	/* Sierra Leone */
    { .icao = "GG", .cc = "GW", .lang = "XX" },	/* Guinea-Bissau */
    { .icao = "GL", .cc = "LR", .lang = "XX" },	/* Liberia */
    { .icao = "GM", .cc = "MA", .lang = "ar" },	/* Morocco */
    { .icao = "GO", .cc = "SN", .lang = "fr" },	/* Senegal */
    { .icao = "GQ", .cc = "MR", .lang = "XX" },	/* Mauritania */
    { .icao = "GS", .cc = "EH", .lang = "XX" },	/* Western Sahara */
    { .icao = "GU", .cc = "GN", .lang = "XX" },	/* Guinea */
    { .icao = "GV", .cc = "CV", .lang = "XX" },	/* Cape Verde */
    { .icao = "HA", .cc = "ET", .lang = "XX" },	/* Ethiopia */
    { .icao = "HB", .cc = "BI", .lang = "XX" },	/* Burundi */
    { .icao = "HC", .cc = "SO", .lang = "XX" },	/* Somalia */
    { .icao = "HD", .cc = "DJ", .lang = "XX" },	/* Djibouti */
    { .icao = "HE", .cc = "EG", .lang = "ar" },	/* Egypt */
    { .icao = "HH", .cc = "ER", .lang = "XX" },	/* Eritrea */
    { .icao = "HK", .cc = "KE", .lang = "sw" },	/* Kenya */
    { .icao = "HL", .cc = "LY", .lang = "ar" },	/* Libya */
    { .icao = "HR", .cc = "RW", .lang = "XX" },	/* Rwanda */
    { .icao = "HS", .cc = "SD", .lang = "XX" },	/* Sudan */
    { .icao = "HS", .cc = "SS", .lang = "XX" },	/* South Sudan */
    { .icao = "HT", .cc = "TZ", .lang = "XX" },	/* Tanzania */
    { .icao = "HU", .cc = "UG", .lang = "XX" },	/* Uganda */
    { .icao = "K", .cc = "US", .lang = "en" },	/* United States */
    { .icao = "LA", .cc = "AL", .lang = "sq" },	/* Albania */
    { .icao = "LB", .cc = "BG", .lang = "bg" },	/* Bulgaria */
    { .icao = "LC", .cc = "CY", .lang = "XX" },	/* Cyprus */
    { .icao = "LD", .cc = "HR", .lang = "hr" },	/* Croatia */
    { .icao = "LE", .cc = "ES", .lang = "es" },	/* Spain */
    { .icao = "LF", .cc = "FR", .lang = "fr" },	/* France */
    { .icao = "LF", .cc = "PM", .lang = "fr" },	/* Saint Pierre and Miquelon */
    { .icao = "LG", .cc = "GR", .lang = "el" },	/* Greece */
    { .icao = "LH", .cc = "HU", .lang = "hu" },	/* Hungary */
    { .icao = "LI", .cc = "IT", .lang = "it" },	/* Italy */
    { .icao = "LJ", .cc = "SI", .lang = "sl" },	/* Slovenia */
    { .icao = "LK", .cc = "CZ", .lang = "cs" },	/* Czech Republic */
    { .icao = "LL", .cc = "IL", .lang = "he" },	/* Israel */
    { .icao = "LM", .cc = "MT", .lang = "mt" },	/* Malta */
    { .icao = "LN", .cc = "MC", .lang = "fr" },	/* Monaco */
    { .icao = "LO", .cc = "AT", .lang = "de" },	/* Austria */
    { .icao = "LP", .cc = "PT", .lang = "pt" },	/* Portugal */
    { .icao = "LQ", .cc = "BA", .lang = "bs" },	/* Bosnia and Herzegovina */
    { .icao = "LR", .cc = "RO", .lang = "ro" },	/* Romania */
    { .icao = "LS", .cc = "CH", .lang = "de" },	/* Switzerland */
    { .icao = "LT", .cc = "TR", .lang = "tr" },	/* Turkey */
    { .icao = "LU", .cc = "MD", .lang = "ro" },	/* Moldova */
    { .icao = "LV", .cc = "PS", .lang = "ar" },	/* Palestine */
    { .icao = "LW", .cc = "MK", .lang = "mk" },	/* Macedonia */
    { .icao = "LX", .cc = "GI", .lang = "en" },	/* Gibraltar */
    { .icao = "LY", .cc = "ME", .lang = "sr" },	/* Montenegro */
    { .icao = "LY", .cc = "RS", .lang = "sr" },	/* Serbia */
    { .icao = "LZ", .cc = "SK", .lang = "sk" },	/* Slovakia */
    { .icao = "MB", .cc = "TC", .lang = "en" },	/* Turks and Caicos Islands */
    { .icao = "MD", .cc = "DO", .lang = "es" },	/* Dominican Republic */
    { .icao = "MG", .cc = "GT", .lang = "es" },	/* Guatemala */
    { .icao = "MH", .cc = "HN", .lang = "es" },	/* Honduras */
    { .icao = "MI", .cc = "VI", .lang = "en" },	/* United States */
						/* Virgin Islands */
    { .icao = "MK", .cc = "JM", .lang = "en" },	/* Jamaica */
    { .icao = "MM", .cc = "MX", .lang = "es" },	/* Mexico */
    { .icao = "MN", .cc = "NI", .lang = "es" },	/* Nicaragua */
    { .icao = "MP", .cc = "PA", .lang = "es" },	/* Panama */
    { .icao = "MR", .cc = "CR", .lang = "es" },	/* Costa Rica */
    { .icao = "MS", .cc = "SV", .lang = "es" },	/* El Salvador */
    { .icao = "MT", .cc = "HT", .lang = "fr" },	/* Haiti */
    { .icao = "MU", .cc = "CU", .lang = "es" },	/* Cuba */
    { .icao = "MW", .cc = "KY", .lang = "en" },	/* Cayman Islands */
    { .icao = "MY", .cc = "BS", .lang = "en" },	/* Bahamas */
    { .icao = "MZ", .cc = "BZ", .lang = "en" },	/* Belize */
    { .icao = "NC", .cc = "CK", .lang = "en" },	/* Cook Islands */
    { .icao = "NE", .cc = "CL", .lang = "es" },	/* Chile */
    { .icao = "NFT", .cc = "TO", .lang = "XX" },/* Tonga */
    { .icao = "NF", .cc = "FJ", .lang = "XX" },	/* Fiji */
    { .icao = "NGF", .cc = "TV", .lang = "XX" },/* Tuvalu */
    { .icao = "NG", .cc = "KI", .lang = "XX" },	/* Kiribati */
    { .icao = "NI", .cc = "NU", .lang = "XX" },	/* Niue */
    { .icao = "NL", .cc = "WF", .lang = "XX" },	/* Wallis and Futuna */
    { .icao = "NS", .cc = "AS", .lang = "en" },	/* American Samoa */
    { .icao = "NS", .cc = "WS", .lang = "XX" },	/* Samoa */
    { .icao = "NT", .cc = "PF", .lang = "fr" },	/* French Polynesia */
    { .icao = "NV", .cc = "VU", .lang = "XX" },	/* Vanuatu */
    { .icao = "NW", .cc = "NC", .lang = "XX" },	/* New Caledonia */
    { .icao = "NZ", .cc = "NZ", .lang = "en" },	/* New Zealand */
    { .icao = "OA", .cc = "AF", .lang = "ps" },	/* Afghanistan */
    { .icao = "OB", .cc = "BH", .lang = "ar" },	/* Bahrain */
    { .icao = "OE", .cc = "SA", .lang = "ar" },	/* Saudi Arabia */
    { .icao = "OI", .cc = "IR", .lang = "fa" },	/* Iran */
    { .icao = "OJ", .cc = "JO", .lang = "ar" },	/* Jordan */
    { .icao = "OJ", .cc = "PS", .lang = "ar" },	/* Palestine */
    { .icao = "OK", .cc = "KW", .lang = "ar" },	/* Kuwait */
    { .icao = "OL", .cc = "LB", .lang = "ar" },	/* Lebanon */
    { .icao = "OM", .cc = "AE", .lang = "ar" },	/* United Arab Emirates */
    { .icao = "OO", .cc = "OM", .lang = "ar" },	/* Oman */
    { .icao = "OP", .cc = "PK", .lang = "ur" },	/* Pakistan */
    { .icao = "OR", .cc = "IQ", .lang = "ar" },	/* Iraq */
    { .icao = "OS", .cc = "SY", .lang = "syr" },/* Syria */
    { .icao = "OT", .cc = "QA", .lang = "ar" },	/* Qatar */
    { .icao = "OY", .cc = "YE", .lang = "ar" },	/* Yemen */
    { .icao = "PA", .cc = "US", .lang = "en" },	/* United States */
    { .icao = "PB", .cc = "US", .lang = "en" },	/* United States */
    { .icao = "PF", .cc = "US", .lang = "en" },	/* United States */
    { .icao = "PG", .cc = "GU", .lang = "en" },	/* Guam */
    { .icao = "PG", .cc = "MP", .lang = "en" },	/* Northern Mariana Islands */
    { .icao = "PH", .cc = "US", .lang = "en" },	/* United States */
    { .icao = "PJ", .cc = "US", .lang = "en" },	/* United States */
    { .icao = "PK", .cc = "MH", .lang = "en" },	/* Marshall Islands */
    { .icao = "PL", .cc = "NZ", .lang = "en" },	/* New Zealand */
    { .icao = "PL", .cc = "US", .lang = "en" },	/* United States */
    { .icao = "PM", .cc = "US", .lang = "en" },	/* United States */
    { .icao = "PO", .cc = "US", .lang = "en" },	/* United States */
    { .icao = "PP", .cc = "US", .lang = "en" },	/* United States */
    { .icao = "PT", .cc = "FM", .lang = "XX" },	/* Federated States of */
						/* Micronesia */
    { .icao = "PT", .cc = "PW", .lang = "XX" },	/* Palau */
    { .icao = "PW", .cc = "US", .lang = "en" },	/* United States */
    { .icao = "RC", .cc = "TW", .lang = "zh" },	/* Taiwan */
    { .icao = "RJ", .cc = "JP", .lang = "ja" },	/* Japan */
    { .icao = "RK", .cc = "KR", .lang = "ko" },	/* South Korea */
    { .icao = "RO", .cc = "JP", .lang = "ja" },	/* Japan */
    { .icao = "RP", .cc = "PH", .lang = "en" },	/* Philippines */
    { .icao = "SA", .cc = "AR", .lang = "es" },	/* Argentina */
    { .icao = "SB", .cc = "BR", .lang = "pt" },	/* Brazil */
    { .icao = "SC", .cc = "CL", .lang = "es" },	/* Chile */
    { .icao = "SD", .cc = "BR", .lang = "pt" },	/* Brazil */
    { .icao = "SE", .cc = "EC", .lang = "es" },	/* Ecuador */
    { .icao = "SF", .cc = "FK", .lang = "en" },	/* Falkland Islands */
    { .icao = "SG", .cc = "PY", .lang = "es" },	/* Paraguay */
    { .icao = "SK", .cc = "CO", .lang = "es" },	/* Colombia */
    { .icao = "SL", .cc = "BO", .lang = "es" },	/* Bolivia */
    { .icao = "SM", .cc = "SR", .lang = "XX" },	/* Suriname */
    { .icao = "SN", .cc = "BR", .lang = "pt" },	/* Brazil */
    { .icao = "SO", .cc = "GF", .lang = "fr" },	/* French Guiana */
    { .icao = "SP", .cc = "PE", .lang = "es" },	/* Peru */
    { .icao = "SS", .cc = "BR", .lang = "pt" },	/* Brazil */
    { .icao = "SU", .cc = "UY", .lang = "es" },	/* Uruguay */
    { .icao = "SV", .cc = "VE", .lang = "es" },	/* Venezuela */
    { .icao = "SW", .cc = "BR", .lang = "pt" },	/* Brazil */
    { .icao = "SY", .cc = "GY", .lang = "XX" },	/* Guyana */
    { .icao = "TA", .cc = "AG", .lang = "XX" },	/* Antigua and Barbuda */
    { .icao = "TB", .cc = "BB", .lang = "XX" },	/* Barbados */
    { .icao = "TD", .cc = "DM", .lang = "XX" },	/* Dominica */
    { .icao = "TF", .cc = "BL", .lang = "fr" },	/* Saint Barthélemy */
    { .icao = "TF", .cc = "GP", .lang = "fr" },	/* Guadeloupe */
    { .icao = "TF", .cc = "MF", .lang = "fr" },	/* Saint Martin */
    { .icao = "TF", .cc = "MQ", .lang = "fr" },	/* Martinique */
    { .icao = "TG", .cc = "GD", .lang = "en" },	/* Grenada */
    { .icao = "TI", .cc = "VI", .lang = "en" },	/* United States */
						/* Virgin Islands */
    { .icao = "TJ", .cc = "PR", .lang = "es" },	/* Puerto Rico */
    { .icao = "TK", .cc = "KN", .lang = "en" },	/* Saint Kitts and Nevis */
    { .icao = "TL", .cc = "LC", .lang = "en" },	/* Saint Lucia */
    { .icao = "TN", .cc = "AW", .lang = "nl" },	/* Aruba */
    { .icao = "TN", .cc = "BQ", .lang = "nl" },	/* Caribbean Netherlands */
    { .icao = "TN", .cc = "CW", .lang = "nl" },	/* Curaçao */
    { .icao = "TN", .cc = "SX", .lang = "nl" },	/* Sint Maarten */
    { .icao = "TQ", .cc = "AI", .lang = "XX" },	/* Anguilla */
    { .icao = "TR", .cc = "MS", .lang = "XX" },	/* Montserrat */
    { .icao = "TT", .cc = "TT", .lang = "en" },	/* Trinidad and Tobago */
    { .icao = "TU", .cc = "VG", .lang = "en" },	/* British Virgin Islands */
    { .icao = "TV", .cc = "VC", .lang = "XX" },	/* Saint Vincent and */
						/* the Grenadines */
    { .icao = "TX", .cc = "BM", .lang = "XX" },	/* Bermuda */
    { .icao = "UA", .cc = "KZ", .lang = "ky" },	/* Kazakhstan */
    { .icao = "UB", .cc = "AZ", .lang = "XX" },	/* Azerbaijan */
    { .icao = "UC", .cc = "KG", .lang = "XX" },	/* Kyrgyzstan */
    { .icao = "UD", .cc = "AM", .lang = "XX" },	/* Armenia */
    { .icao = "UE", .cc = "RU", .lang = "ru" },	/* Russia */
    { .icao = "UG", .cc = "GE", .lang = "ka" },	/* Georgia */
    { .icao = "UH", .cc = "RU", .lang = "ru" },	/* Russia */
    { .icao = "UI", .cc = "RU", .lang = "ru" },	/* Russia */
    { .icao = "UK", .cc = "UA", .lang = "uk" },	/* Ukraine */
    { .icao = "UL", .cc = "RU", .lang = "ru" },	/* Russia */
    { .icao = "UM", .cc = "BY", .lang = "ru" },	/* Belarus */
    { .icao = "UN", .cc = "RU", .lang = "ru" },	/* Russia */
    { .icao = "UO", .cc = "RU", .lang = "ru" },	/* Russia */
    { .icao = "UR", .cc = "RU", .lang = "ru" },	/* Russia */
    { .icao = "US", .cc = "RU", .lang = "ru" },	/* Russia */
    { .icao = "UT", .cc = "TJ", .lang = "tg" },	/* Tajikistan */
    { .icao = "UT", .cc = "TM", .lang = "XX" },	/* Turkmenistan */
    { .icao = "UT", .cc = "UZ", .lang = "uz" },	/* Uzbekistan */
    { .icao = "UU", .cc = "RU", .lang = "ru" },	/* Russia */
    { .icao = "UW", .cc = "RU", .lang = "ru" },	/* Russia */
    { .icao = "VA", .cc = "IN", .lang = "hi" },	/* India */
    { .icao = "VB", .cc = "MM", .lang = "XX" },	/* Myanmar */
    { .icao = "VC", .cc = "LK", .lang = "XX" },	/* Sri Lanka */
    { .icao = "VD", .cc = "KH", .lang = "XX" },	/* Cambodia */
    { .icao = "VE", .cc = "IN", .lang = "hi" },	/* India */
    { .icao = "VG", .cc = "BD", .lang = "XX" },	/* Bangladesh */
    { .icao = "VH", .cc = "HK", .lang = "zh" },	/* Hong Kong */
    { .icao = "VI", .cc = "IN", .lang = "hi" },	/* India */
    { .icao = "VL", .cc = "LA", .lang = "XX" },	/* Laos */
    { .icao = "VM", .cc = "MO", .lang = "zh" },	/* Macau */
    { .icao = "VN", .cc = "NP", .lang = "XX" },	/* Nepal */
    { .icao = "VO", .cc = "IN", .lang = "hi" },	/* India */
    { .icao = "VQ", .cc = "BT", .lang = "XX" },	/* Bhutan */
    { .icao = "VR", .cc = "MV", .lang = "div" },/* Maldives */
    { .icao = "VT", .cc = "TH", .lang = "th" },	/* Thailand */
    { .icao = "VV", .cc = "VN", .lang = "vi" },	/* Vietnam */
    { .icao = "VY", .cc = "MM", .lang = "XX" },	/* Myanmar */
    { .icao = "WA", .cc = "ID", .lang = "id" },	/* Indonesia */
    { .icao = "WB", .cc = "BN", .lang = "ms" },	/* Brunei */
    { .icao = "WB", .cc = "MY", .lang = "ms" },	/* Malaysia */
    { .icao = "WI", .cc = "ID", .lang = "id" },	/* Indonesia */
    { .icao = "WM", .cc = "MY", .lang = "ms" },	/* Malaysia */
    { .icao = "WP", .cc = "TL", .lang = "XX" },	/* Timor-Leste */
    { .icao = "WS", .cc = "SG", .lang = "zh" },	/* Singapore */
    { .icao = "YP", .cc = "CX", .lang = "XX" },	/* Christmas Island */
    { .icao = "Y", .cc = "AU", .lang = "en" },	/* Australia */
    { .icao = "ZB", .cc = "CN", .lang = "zh" },	/* China */
    { .icao = "ZG", .cc = "CN", .lang = "zh" },	/* China */
    { .icao = "ZH", .cc = "CN", .lang = "zh" },	/* China */
    { .icao = "ZJ", .cc = "CN", .lang = "zh" },	/* China */
    { .icao = "ZK", .cc = "KP", .lang = "ko" },	/* North Korea */
    { .icao = "ZL", .cc = "CN", .lang = "zh" },	/* China */
    { .icao = "ZM", .cc = "MN", .lang = "mn" },	/* Mongolia */
    { .icao = "ZP", .cc = "CN", .lang = "zh" },	/* China */
    { .icao = "ZS", .cc = "CN", .lang = "zh" },	/* China */
    { .icao = "ZT", .cc = "CN", .lang = "zh" },	/* China */
    { .icao = "ZU", .cc = "CN", .lang = "zh" },	/* China */
    { .icao = "ZW", .cc = "CN", .lang = "zh" },	/* China */
    { .icao = "ZY", .cc = "CN", .lang = "zh" },	/* China */
    { .icao = NULL, .cc = NULL, .lang = NULL }	/* Last entry */
};

/*
 * Converts an ICAO code to a country code. This performs a simple prefix
 * match using the icao2cc_table.
 */
const char *
icao2cc(const char *icao)
{
	/*
	 * Doing a linear search is not particularly elegant, but the size
	 * of the ICAO table is fixed and small, so it probably doesn't
	 * matter anyway.
	 */
	for (int i = 0; icao2cc_table[i].icao != NULL; i++) {
		if (strstr(icao, icao2cc_table[i].icao) == icao)
			return (icao2cc_table[i].cc);
	}

	return (NULL);
}

/*
 * Grabs an ICAO airport code and tries to map it to language code of
 * the principal language spoken at that airport. This shouldn't be relied
 * upon to be very accurate, since in reality the airport-to-language
 * mapping is anything but clear cut.
 * Returns a two- or three-letter language code (if no two-letter one exists),
 * or "XX" if no suitable mapping was found.
 */
const char *
icao2lang(const char *icao)
{
	for (int i = 0; icao2cc_table[i].icao != NULL; i++) {
		if (strcmp(icao, icao2cc_table[i].icao) == 0)
			return (icao2cc_table[i].lang);
	}
	return ("XX");
}
