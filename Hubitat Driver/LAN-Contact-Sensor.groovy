import groovy.json.JsonSlurper

metadata {
	definition (name: "HTTP Test Switch", namespace: "pprovost", author: "Peter Provost") {
        capability "Contact Sensor"
        capability "Sensor"
        capability "Signal Strength"
        
        attribute "lastUpdated", "String"
	}

	preferences {
		input(name: "deviceIP", type: "string", title:"Device IP Address", description: "Enter IP Address of your HTTP server", required: true, displayDuringSetup: true)
		input(name: "devicePort", type: "string", title:"Device Port", description: "Enter Port of your HTTP server (defaults to 80)", defaultValue: "80", required: false, displayDuringSetup: true)
	}
}

def parse(String description) {
    log.debug "parse(${description}) called"
    
    def msg = parseLanMessage(description);
    log.debug "msg: ${msg}"

    def isMap = msg instanceof Map
    
    def results = []
    
    if (msg['body'].startsWith("{"))
    {
        def obj = new JsonSlurper().parseText(msg['body'])
        if (obj instanceof Map)
        {
            obj.each {
                key, value -> if (key && value)
                	results << createEvent(name: key, value: value)
            }
        }
    }
    else
    {
        def parts = description.split(" ")
        def name  = parts.length>0?parts[0].trim():null
        def value = parts.length>1?parts[1].trim():null
        if (name && value) {
            // handle rssi using the format sent by the SmartThingsESP8266WiFi class
            if (name.startsWith("rssi")) {
	           	results << createEvent(name: name, value: value, displayed: false)
            }
                
            // Update lastUpdated date and time
            def nowDay = new Date().format("MMM dd", location.timeZone)
            def nowTime = new Date().format("h:mm a", location.timeZone)
            sendEvent(name: "lastUpdated", value: nowDay + " at " + nowTime, displayed: false)
        }
        else {
            log.debug "Missing either name or value.  Cannot parse!"
        }
    }
    
    return results
}