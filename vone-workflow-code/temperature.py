# Access the data list directly from parameter[1]
data_list = parameter[1]  # parameter[1] is expected to be a list of sensor data
threshold = 35  # Define temperature threshold

if len(data_list) > 0:
    # Get the latest data entry
    latest_data = data_list[-1]  # Access the last (most recent) entry
    temp = latest_data["Temperature"] if "Temperature" in latest_data else None  # Safely access "Temperature"

    # Check if the temperature is valid and over the threshold
    if temp is not None and temp.isdigit() and int(temp) > threshold:
        # Prepare the alert message
        msgbody = f'<p>The current temperature value {temp} is over threshold, the reading is more than {threshold}.</p><br>'
        output[1] = "[Warning] Temperature Reading Over Threshold!"
        output[2] = msgbody
        output[3] = 2  # Trigger for the next component
    elif temp is None:
        # Handle missing temperature data
        output[1] = "[Error] Temperature Data Missing"
        output[2] = "<p>Temperature data is missing or not available.</p><br>"
        output[3] = 0  # Indicate an error state
    else:
        # Temperature is within acceptable range
        output[1] = "[Info] Temperature is Normal"
        output[2] = f"<p>The current temperature value is {temp}, which is below the threshold of {threshold}.</p><br>"
        output[3] = 1  # Indicate no action needed
else:
    # Handle case where no data is available
    output[1] = "[Info] No Data Available"
    output[2] = "<p>No data available to check the temperature.</p><br>"
    output[3] = 0  # Indicate no data condition
