<html>
<head>
    <style type="text/css">
    </style>
    
    <script type="text/javascript" src="utils.js"></script>
    <script type="text/JavaScript">
    
    	/* return the core object */
        function getCore(){
            return document.getElementById('core');
        }
        
        /* Registration */
        function registration(username,password,server){
        	var core = getCore();
        	
        	/*create proxy config*/        	
			var proxy = core.newProxyConfig();
			
			/*create authentication structure from identity*/
			var authinfo = core.newAuthInfo(username, null,password,null,null);
			/*add authentication info to LinphoneCore*/
			core.addAuthInfo(authinfo);
			
			/*configure proxy entries*/
			proxy.identity = 'sip:' + username + '@' + server; /*set identity with user name and domain*/
			proxy.serverAddr = 'sip:' + server; /* we assume domain = proxy server address*/
			proxy.registerEnabled = true; /*activate registration for this proxy config*/
			core.addProxyConfig(proxy); /*add proxy config to linphone core*/
			core.defaultProxy = proxy; /*set to default proxy*/
        }
        
		function loadCore(){
			var core = getCore();
			core.init();
				
	        /* Add callback for registration and call state */
	        addEvent(core,"registrationStateChanged",onRegistrationStateChanged);
	           
	        /* Display logs information in the console */
	        core.logHandler = function(level, message) {
	           	window.console.log(message);
	        }
	        
	        /* Start main loop for receiving notifications and doing background linphonecore work */
	        core.iterateEnabled = true;
		}
	        
		/* Callback that display registration states */
		function onRegistrationStateChanged(event, proxy, state, message){
			updateStatus('statusR',message);
        	console.log(message);
        }
        
        /* Hnadler function */
        function onRegistration(form1){
			registration(document.form1.username.value,document.form1.password.value,document.form1.address.value);
		}
    </script>
    
</head>
<body>
	<object id="core" type="application/x-linphone-web" width="0" height="0">
	 	<param name="onload" value='loadCore'>
	</object>
	
	<form name="form1">
		<p id="Registration"> 1. Registration</p>
		<div>Username : <input id="username" type="text" value="" /></div>
		<div>Password : <input id="password" type="password" value=""/></div>
		<div>Server : <input id="address" type="text" value="" /></div>
		<input type="button" OnClick="onRegistration(form1)" value="Register">
		<div> Status : <span id="statusR">No registration</span></div>
	</form>
</body>
</html>