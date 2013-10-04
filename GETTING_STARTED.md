# Getting started

## Loading and initializing the plugin

### Loading the plugin

**TODO**

### Initializing the plugin

To initialize the plugin, call the init() method of the core object:

	core.init();

For more information, see the documentation of the init() method of [LinphoneCore](external-LinphoneCore.html).

### Registering the callback handlers

To be able to handle events coming from the plugin, some callback handlers need to be registered.
Here is an example with a function displaying the call state changes in the javascript console:

	function onCallStateChanged(event, call, state, message) {
		console.log(message);
	}
	addEvent(core, "callStateChanged", onCallStateChanged);

### Starting iterate timer

	core.iterateEnabled = true;

## Where to go from here?

All the operations are then performed by calling methods of the core plugin object.
The core object has methods to create new objects such as [LinphoneAddress](external-LinphoneAddress.html), [LinphoneAuthInfo](external-LinphoneAuthInfo.html), [LinphoneFriend](external-LinphoneFriend.html)...

### How to register to a SIP server?

	function registration(username, password, server) {
		var core = getCore();
		var proxy = core.newProxyConfig();
		var authinfo = core.newAuthInfo(username, null, password, null, null);
		core.addAuthInfo(authinfo);
		proxy.identity = 'sip:' + username + '@' + server;
		proxy.serverAddr = 'sip:' + server;
		proxy.expires = 3600;
		proxy.registerEnabled = true;
		core.addProxyConfig(proxy);
		core.defaultProxy = proxy;
	}

For more information about the registration process, look at the documentation of the [LinphoneProxyConfig](external-LinphoneProxyConfig.html) and [LinphoneAuthInfo](external-LinphoneAuthInfo.html) objects.

### How to initiate an outgoing call?

Here is a simple javascript function to initiate an outgoing call:

	function call(addressStr) {
		var core = getCore();
		var address = core.newAddress(addressStr);
		if (address !== null) {
			core.inviteAddress(address);
			console.log("Call: " + address.asString());
		}
	}

For more information about calls, look at the documentation of the [LinphoneAddress](external-LinphoneAddress.html) and [LinphoneCall](external-LinphoneCall.html) objects.
