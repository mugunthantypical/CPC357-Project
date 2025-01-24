# Extract the list from the parameter
data_list = parameter[1]  # This is the list of sensor data
threshold = 30

if len(data_list) > 0:
    # Get the last item in the list
    latest_data = data_list[-1]  # Access the last entry (most recent reading)

    # Get the soil moisture value
    moisture = latest_data.get("Soil moisture", "0")  # Default to "0" if key is missing

    # Check if moisture is below the threshold
    if int(moisture) < threshold:
        msgbody = '<p>The current soil moisture ' + moisture + ' is below threshold, the reading is less than ' + str(threshold) + '.</p><br>'
        output[1] = "[Warning] Your Plants Need Water!"
        output[2] = msgbody
        output[3] = 2
else:
    # Handle case where data_list is empty
    output[1] = "[Info] No data available."
    output[2] = "<p>No data available to check soil moisture levels.</p><br>"
    output[3] = 1 