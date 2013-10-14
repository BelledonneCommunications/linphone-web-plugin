# Getting started

## Loading and initializing the plugin

### Accessing the enumeration values from the plugin

To access the enumeration values that need to be passed as arguments and/or returned by the plugin methods and properties you need to include the *linphone.js* file that is delivered in the SDK package in your project.

### Loading the plugin

**TODO**

### Initializing the plugin

To initialize the plugin, call the [init method of the core object](external-LinphoneCore.html#init):

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
		}
	}

For more information about calls, look at the documentation of the [LinphoneAddress](external-LinphoneAddress.html) and [LinphoneCall](external-LinphoneCall.html) objects.

### How to initiate an outgoing call with custom SIP headers?

Here is an example showing the use of the custom SIP headers when initiating an outgoing call.

	function callWithCustomHeaders(addressStr) {
		var core = getCore();
		var params = core.createDefaultCallParameters();
		if (params !== null) {
			params.addCustomHeader("Weather", "bad");
			params.addCustomHeader("Working", "yes");
			var headerValue = params.getCustomHeader("Weather"); // headerValue = "bad"
		}
		var address = core.newAddress(addressStr);
		if (address !== null) {
			core.inviteAddressWithParams(address, params);
		}
	}

For more information about custom SIP headers, look at [LinphoneCallParams.addCustomHeader](external-LinphoneCallParams.html#addCustomHeader)
and [LinphoneCallParams.getCustomHeader](external-LinphoneCallParams.html#getCustomHeader).

### How to handle presence?

Your own presence and the presence of your friends are handled by the [LinphonePresenceModel](external-LinphonePresenceModel.html) object.
Some helper properties and methods are there to handle the basic cases of presence, that is to say having a single activity.

For example to set your own presence to "on-the-phone":

	function onThePhone() {
		var core = getCore();
		var presenceModel = core.presenceModel;
		presenceModel.setActivity(linphone.PresenceActivityType.OnThePhone, null);
		core.presenceModel = presenceModel;
	}

The same can be done with more control by creating the whole presence model manually. Here is the same example but doing everything manually:

	function onThePhone() {
		var core = getCore();
		var model = core.newPresenceModel();
		var service = core.newPresenceService(null, linphone.PresenceBasicStatus.Closed, null);
		model.addService(service);
		var person = core.newPresencePerson(null);
		var activity = core.newPresenceActivity(linphone.PresenceActivityType.OnThePhone, null);
		person.addActivity(activity);
		model.addPerson(person);
		core.presenceModel = model;
	}

This is basically the same for the presence of your friends except that you cannot modify it.
So you can access the presence information of a friend with some helper properties and methods:

	function printFriendPresence(lf) {
		var model = lf.presenceModel;
		if (model !== null) {
			console.log("Basic status: " + model.basicStatus);
			console.log("Activity: " + model.activity);
		} else {
			console.log("Friend " + lf.name + " is offline.");
		}
	}

But you can also explore the whole presence model manually. To see all the methods and properties to do this, look at the documentation of the
[LinphonePresenceActivity](external-LinphonePresenceActivity.html), [LinphonePresenceModel](external-LinphonePresenceModel.html),
[LinphonePresenceNote](external-LinphonePresenceNote.html), [LinphonePresencePerson](external-LinphonePresencePerson.html) and
[LinphonePresenceService](external-LinphonePresenceService.html) objects.
