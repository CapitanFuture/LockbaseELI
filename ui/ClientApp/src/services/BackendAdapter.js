// fetch all the data from the backend or mock it 
import React from 'react';

import messages_de from './../translations/de.json';
import messages_en from './../translations/en.json';

// Languages 

export const languages = [
	{ value: 'de', label: 'Deutsch', messages: messages_de },
	{ value: 'en', label: 'English', messages: messages_en },
	{ value: 'fr', label: 'Français' },
	{ value: 'it', label: 'Italiano' },
	{ value: 'es', label: 'Español' }
]

export function findLanguage (language) {
	// console.log("find languagee" + language.locale);
	const index = languages.findIndex(x => x.value === language.locale);
	return languages[index];
};


export let i8n_de = {
	locale: 'de',
	messages: messages_de
}

export let i8n_en = {
	locale: 'en',
	messages: messages_en
}

export const LanguageContext = React.createContext(
	{
		language: i18nConfig,
		switchLanguage: (selected) => { },
	}
);

// persons and departments 

export const persons = [
	{ value: '900-1', label: 'Ahrens, Andrea', department: 'Geschäftsführung' },
	{ value: '901-1', label: 'Barthauer, Thomas', department: 'Geschäftsführung' },
	{ value: '103-1', label: 'Fendler, Klaus', department: 'Buchhaltung' },
	{ value: '104-1', label: 'Kistler, Sabine', department: 'Vertrieb' },
	{ value: '105-1', label: 'Kohl, Ulrich', department: 'Vertrieb' },
	{ value: '200-1', label: 'Leinkamp, Sebastian', department: 'Lager' },
	{ value: '201-1', label: 'Mertens, Martina', department: 'Lager' },
	{ value: '202-1', label: 'Sidow, Janin', department: 'Montage' },
	{ value: '203-1', label: 'Walter, Jens', department: 'Montage' },
	{ value: '203-2', label: 'Winter, Sina', department: 'Montage' },
	{ value: '203-3', label: 'Wondraschek, Volker', department: 'Montage' }
]

// doors and locks 

export const doors = [
	{ value: 'W1', label: 'Tor West', building: '-.-', image: 'torwest' },
	{
		type: 'group', name: 'Verwaltung', items: [
			{ value: '100', label: 'Konferenzraum', building: 'Verwaltung', image: 'konferenzraum' },
			{ value: '101', label: 'Büro Ahrens', building: 'Verwaltung', image: 'buero_ahrens' },
			{ value: '102', label: 'Büro Barthauer', building: 'Verwaltung', image: 'buero_barthauer' },
			{ value: '103', label: 'Buchhaltung', building: 'Verwaltung', image: 'buchhaltung' },
			{ value: '104', label: 'Büro Vertrieb 1', building: 'Verwaltung', image: 'buero_vertrieb1' },
			{ value: '105', label: 'Büro Vertrieb 2', building: 'Verwaltung', image: 'buero_vertrieb2' },
			{ value: 'Z1', label: 'Eingang West', building: 'Verwaltung', image: 'eingang_west' }
		]
	},
	{
		type: 'group', name: 'Produktion', items: [
			{ value: '204', label: 'Werkhalle West', building: 'Produktion', image: 'werkhalle_west' },
			{ value: '200', label: 'Metalllager', building: 'Produktion', image: 'metalllager' },
			{ value: '202', label: 'Büro Montage', building: 'Produktion', image: 'buero_montage' },
			{ value: '201', label: 'Warenlager', building: 'Produktion', image: 'warenlager' },
			{ value: '205', label: 'Werkhalle Süd', building: 'Produktion', image: 'werkhalle_sued' }
		]
	}
]


// Time functions 

function* hoursGenerator() {
	var index = 0;
	while (index < 24) {
		yield index++;
	}
}

function* minutesGenerator() {
	var index = 0;
	while (index < 4) {
		yield index * 15;
		index++;
	}
}

export let hours = Array
	.from(hoursGenerator())
	.map(x => {
		return {
			value: x,
			label: x.toString().padStart(2, '0')
		};
	})

export let minutes = Array
	.from(minutesGenerator())
	.map(x => {
		return {
			value: x,
			label: x.toString().padStart(2, '0')
		};
	})
