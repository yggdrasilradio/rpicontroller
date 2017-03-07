var xhrRequest = function (url, type, callback) {

        var xhr = new XMLHttpRequest();
        xhr.onload = function () {
                callback(this.responseText);
        };
        xhr.open(type, url);
        xhr.send();
};

function getWebdata(message) {

        // Construct URL
        var url = "http://example.com/rpilights.php?id=" + message;

        // Send web request
		console.log(url);
        xhrRequest(url, 'GET',
                function(responsetext) {
                        console.log("Webdata: " + responsetext);

			// Send response to Pebble
			var dictionary = { "KEY_RESPONSE": responsetext };
			console.log("Sending web response to Pebble");
			Pebble.sendAppMessage(dictionary, function(e) {
				console.log("Web response sent to Pebble successfully!");
			},
			function(e) {
				console.log("Error sending web response to Pebble!");
			});
                }
        );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
        function(e) {
                console.log("PebbleKit JS ready!");
		getWebdata('status');
});

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage', function(e) {

	var value = e.payload.KEY_REQUEST;
	console.log("AppMessage " + value);
	getWebdata(value);
});
