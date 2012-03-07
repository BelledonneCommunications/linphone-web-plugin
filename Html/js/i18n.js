jQuery.i18n = {}
jQuery.i18n.locale = ''
jQuery.i18n.data = {}
jQuery.i18n.change = function(locale) {
	if (locale != jQuery.i18n.locale) {
		jQuery.i18n.locale = locale
		console.log('Change locale: ' + jQuery.i18n.locale)
		jQuery('div .lang').each(function() {
			var element = $(this)
			element.html(jQuery.i18n.translate(element.attr('title')))
		})
	}
}
jQuery.i18n.translate = function(text) {
	var parts = text.split('.')
	var data = jQuery.i18n.data
	var translated_text = '$' + text + '$'
	while (parts.length && data) {
		var part = parts.shift()
		data = data[part]
	}

	if (data)
		data = data[jQuery.i18n.locale]

	if (data) {
		translated_text = data
	}

	translated_text = jQuery('<div/>').text(translated_text).html();

	return translated_text
}
jQuery.i18n.get = function(text) {
	var translated_text = jQuery.i18n.translate(text)
	return '<div class="lang" title="' + text + '">' + translated_text + '</div>'
}