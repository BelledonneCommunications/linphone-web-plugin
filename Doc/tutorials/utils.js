/* config of the plugin with the links for downloads and the version of the plugin */
function getConfig(){  
	var version = '0.0.2.10';
	var config = {
		version : version,
		files: {
			'Windows' : {
				'x86' : {
					'Explorer' : {
						file: 'http://web.linphone.org/downloads/linphone-web-' + version + '-Linux-x86.xpi',
						icon: 'style/img/linphone.ico'
					},
					'DEFAULT' : 'http://web.linphone.org/downloads/linphone-web-' + version + '-Win32.msi'
				},
				'x86_64' : {
					'Explorer' : {
						file: 'http://web.linphone.org/downloads/linphone-web-' + version + '-Linux-x86.xpi',
						icon: 'style/img/linphone.ico'
					},
					'DEFAULT' : 'http://web.linphone.org/downloads/linphone-web-' + version + '-Win32.msi'
				}
			},
			'Linux' : {
				'x86' : {
					'Firefox' : {
						file: 'http://web.linphone.org/downloads/linphone-web-' + version + '-Linux-x86.xpi',
						icon: 'style/img/linphone.ico'
					},
					'DEFAULT' : 'http://web.linphone.org/downloads/linphone-web-' + version + '-Linux-x86.tar.gz'
				}, 
				'x86_64' : {
					'Firefox' : {
						file: 'http://web.linphone.org/downloads/linphone-web-' + version + '-Linux-x86_64.xpi',
						icon: 'style/img/linphone.ico'
					},
					'DEFAULT' : 'http://web.linphone.org/downloads/linphone-web-' + version + '-Linux-x86_64.tar.gz'
				}
			},
			'Mac' : {
				'x86' : {
					'Firefox' : {
						file: 'http://web.linphone.org/downloads/linphone-web-' + version + '-Mac-x86.xpi',
						icon: 'style/img/linphone.ico'
					},
					'DEFAULT' : 'http://web.linphone.org/downloads/linphone-web-' + version + '-Mac-x86.pkg'
				}, 
				'x86_64' : {
					'Firefox' : {
						file: 'http://web.linphone.org/downloads/linphone-web-' + version + '-Mac-x86.xpi',
						icon: 'style/img/linphone.ico'
					},
					'DEFAULT' : 'http://web.linphone.org/downloads/linphone-web-' + version + '-Mac-x86.pkg'
				}
			}
		}
	};
	return config;
}


/* Method to create callbacks */
function addEvent(obj, name, func){
	var browser = searchString(getDataBrowser());
    if (browser !== 'Explorer') {
        obj.addEventListener(name, func, false); 
    } else {
        obj.attachEvent('on' + name, func);
    }
}

/* Function that display some text in a html element*/
function updateStatus(id,value){
	document.getElementById(id).innerHTML= value;
}

/* Method to set the link for downloaded the correct version of the plugin */
function setPluginLink(config,det){
	config.file = {};
	
	if (typeof config.files[det.os] !== 'undefined') {
		if (typeof config.files[det.os][det.arch][det.browser] !== 'undefined') {
			config.file.description = config.files[det.os][det.browser];
			config.file.browser = det.browser;
		} else {
			config.file.description = config.files[det.os][det.arch].DEFAULT;
			config.file.browser = 'DEFAULT';
		}
	}
}

function getDataBrowser(){
	var dataBrowser = [
		{
			string: navigator.userAgent,
			subString: "Chrome",
			identity: "Chrome"
		},
		{ 	string: navigator.userAgent,
			subString: "OmniWeb",
			versionSearch: "OmniWeb/",
			identity: "OmniWeb"
		},
		{
			string: navigator.vendor,
			subString: "Apple",
			identity: "Safari",
			versionSearch: "Version"
		},
		{
			prop: window.opera,
			identity: "Opera"
		},
		{
			string: navigator.vendor,
			subString: "iCab",
			identity: "iCab"
		},
		{
			string: navigator.vendor,
			subString: "KDE",
			identity: "Konqueror"
		},
		{
			string: navigator.userAgent,
			subString: "Firefox",
			identity: "Firefox"
		},
		{
			string: navigator.vendor,
			subString: "Camino",
			identity: "Camino"
		},
		{		// for newer Netscapes (6+)
			string: navigator.userAgent,
			subString: "Netscape",
			identity: "Netscape"
		},
		{
			string: navigator.userAgent,
			subString: "MSIE",
			identity: "Explorer",
			versionSearch: "MSIE"
		},
		{
			string: navigator.userAgent,
			subString: "Gecko",
			identity: "Mozilla",
			versionSearch: "rv"
		},
		{ 		// for older Netscapes (4-)
			string: navigator.userAgent,
			subString: "Mozilla",
			identity: "Netscape",
			versionSearch: "Mozilla"
		}
	];
	return dataBrowser;
}

function getDataOS(){
	var dataOS = [
		{
			string: navigator.platform,
			subString: "Win",
			identity: "Windows"
		},
		{
			string: navigator.platform,
			subString: "Mac",
			identity: "Mac"
		},
		{
			string: navigator.userAgent,
			subString: "iPhone",
			identity: "iPhone/iPod"
	    },
		{
			string: navigator.platform,
			subString: "Linux",
			identity: "Linux"
		}
	];
	return dataOS;
}

function browserDetect() {
	var dataBrowser = getDataBrowser();
	var dataOS = getDataOS();
	var object = {
		browser : null,
		version : null ,
		os : null,
		arch : null
	};

	object.browser = searchString(dataBrowser) || "An unknown browser";
	object.version = searchVersion(navigator.userAgent)
		|| searchVersion(navigator.appVersion)
		|| "an unknown version";
	object.os = searchString(dataOS) || "an unknown OS";
	object.arch = searchArch();
	
	return object;
}

function searchString(data) {
	for (var i=0;i<data.length;i++)	{
		var dataString = data[i].string;
		var dataProp = data[i].prop;
		this.versionSearchString = data[i].versionSearch || data[i].identity;
		if (dataString) {
			if (dataString.indexOf(data[i].subString) != -1)
				return data[i].identity;
		}
		else if (dataProp)
			return data[i].identity;
	}
}

function searchArch() {
	var lcua = navigator.userAgent.toLowerCase();
	if(lcua.indexOf("linux x86_64") != -1 || lcua.indexOf("win64") != -1) {
		return "x86_64";
	} else {
		return "x86";
	}
}

function searchVersion(dataString) {
	var index = dataString.indexOf(this.versionSearchString);
	if (index == -1) return;
	return parseFloat(dataString.substring(index+this.versionSearchString.length+1));
}

function isValid(core){
	return typeof core !== 'undefined' && typeof core.valid !== 'undefined' && core.valid;
}

function outdated(actual, plugin) {
	var version1 = actual.split('.');
	var version2 = plugin.split('.');
	for(var i = 0; i < version1.length && i < version2.length; ++i) {
		if(i >= version1.length) {
			return false;
		}
		if(i >= version2.length) {
			return true;
		}
		var number1 = parseInt(version1[i], 10);
		var number2 = parseInt(version2[i], 10);
		if(number2 < number1) {
			return true;
		} else if(number2 > number1) {
			return false;
		}
	}
	return false;
}

function detect(config,core) {
	if (isValid(core)) {
		if(!outdated(config.version, core.pluginVersion)) {
			return 1;
		} else {
			// Browser update
			if (config.file.browser === 'Firefox') {
				if (InstallTrigger.updateEnabled()) {
					InstallTrigger.install({
						'Linphone-Web': {
							URL: config.file.description.file,
							IconURL: config.file.description.icon
						}
					});
				}
			}
			return 0;
		}
	} else {
		if(typeof config.file.description !== 'undefined'){		
			// Browser installation
			if (config.file.browser === 'Firefox') {
				if (InstallTrigger.updateEnabled()) {
					InstallTrigger.install({
					'Linphone-Web': {
							URL: config.file.description.file,
						IconURL: config.file.description.icon
						}
					});
				}
			}
		}
		return 0;
	}
}