# Access the data list directly from parameter[1]
data_list = parameter[1]  # parameter[1] is expected to be a list of sensor data
threshold = 50  # Define humidity threshold

if len(data_list) > 0:
    # Get the latest data entry
    latest_data = data_list[-1]  # Access the last (most recent) entry
    humd = latest_data["Humidity"] if "Humidity" in latest_data else None  # Safely access "Humidity"

    # Check if humidity is valid and below the threshold
    if humd is not None and humd.isdigit() and int(humd) < threshold:
        # Prepare the alert message
        msgbody = f'<p>The current humidity value {humd} is below threshold, the reading is less than {threshold}.</p><br>'
        output[1] = "[Warning] Humidity Reading Below Threshold!"
        output[2] = msgbody
        output[3] = 2  # Trigger for the next component
    elif humd is None:
        # Handle missing humidity data
        output[1] = "[Error] Humidity Data Missing"
        output[2] = "<p>Humidity data is missing or not available.</p><br>"
        output[3] = 0  # Indicate an error state
    else:
        # Humidity is within acceptable range
        output[1] = "[Info] Humidity is Normal"
        output[2] = f"<p>The current humidity value is {humd}, which is above the threshold of {threshold}.</p><br>"
        output[3] = 1  # Indicate no action needed
else:
    # Handle case where no data is available
    output[1] = "[Info] No Data Available"
    output[2] = "<p>No data available to check humidity levels.</p><br>"
    output[3] = 0  # Indicate no data condition
