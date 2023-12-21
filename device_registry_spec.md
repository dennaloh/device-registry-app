# Device Registry REST API

The REST API manages devices and locations, and provides endpoints for listing, filtering, adding, updating, and deleting devices and locations.

## GET /devices
- **Description**: retrieves a list of all devices
- **Operation**: read
- **Return**: json array containing the metadata for each device or `404 not found` when there are no devices found in devices table
### Request
#### Example
```
http://localhost:8080/devices
```
### Response
#### Example
  ```json
  [
    {
        "creation_date": "2023-12-13",
        "location_id": 2,
        "location_name": "LocationB",
        "location_type": "LocationTypeB",
        "name": "DeviceB",
        "serial_number": "1a",
        "type": "TypeB"
    },
    {
        "creation_date": "2023-12-14",
        "location_id": 3,
        "location_name": "location c",
        "location_type": "location type c",
        "name": "device c",
        "serial_number": "1ab",
        "type": "type c"
    },
    ...
  ]
```
**Fields**
- **serial_number**: serial number of device
- **name**: name of device
- **type**: type of device
- **creation_date**: date of creation of row in devices table
- **location_id**: ID of location
- **location_name**: name of location
- **location_type**: type of location

## GET /devices/filter
- **Description**: filters devices based on specified parameters
- **Operation**: read
- **Return**: json array containing the metadata for devices matching the filters or `404 not found` or `400 invalid`
### Parameters
At least one of these parameters must be indicated:
- **serial_number**: serial number of device, alphanumeric string type
- **name**: name of device, string type
- **type**: type of device, string type
- **creation_date**: date of creation of row in devices table, string type with YYYY-MM-DD format
- **location_id**: ID of location, integer type
- **start_date**: start date for filtering creation dates from this date onwards, string type with YYYY-MM-DD format
- **end_date**: end date for filtering creation dates until this date, string type with YYYY-MM-DD format
- **location_name**: name of location, string type
- **location_type**: type of location, string type
### Request
#### Example
```
http://localhost:8080/devices/filter?start_date=2023-12-13&end_date=2023-12-14
```
### Response
#### Example
  ```json
  [
    {
        "creation_date": "2023-12-13",
        "location_id": 2,
        "location_name": "LocationB",
        "location_type": "LocationTypeB",
        "name": "DeviceB",
        "serial_number": "1a",
        "type": "TypeB"
    },
    {
        "creation_date": "2023-12-14",
        "location_id": 3,
        "location_name": "location c",
        "location_type": "location type c",
        "name": "device c",
        "serial_number": "1ab",
        "type": "type c"
    },
    ...
  ]
```
**Fields**
- **serial_number**: serial number of device
- **name**: name of device
- **type**: type of device
- **creation_date**: date of creation of row in devices table
- **location_id**: ID of location
- **location_name**: name of location
- **location_type**: type of location

## POST /devices
- **Description**: creates a new device entry
- **Operation**: create
- **Return**: json containing status and message indicating `200 success` or `400 invalid` or `409 conflict` or `404 not found` or `500 error`
### Parameters
- **serial_number**: serial number of device, alphanumeric string type
- **name**: name of device, string type
- **type**: type of device, string type
- **creation_date** (optional): date of creation of row in devices table, string type with YYYY-MM-DD format (if not provided, today's date is assigned)
- **location_id**: ID of location, integer type
### Request
#### Example
```
http://localhost:8080/devices?serial_number=12345&name=DeviceA&type=TypeA&location_id=1
```
### Response
#### Example
```json
{
  "status": "success",
  "message": "New device created successfully: 12345 | DeviceA | TypeA | 2023-01-15 | 1"
}
```
**Fields**
- **status**: status of response
- **message**: message indicating success or cause of error

## PATCH /devices/{serial_number}
- **Description**: updates an existing device by serial number
- **Operation**: update
- **Return**: json containing status and message indicating `200 success` or `400 invalid` or `404 not found` or `500 error`
### Parameters
At least one of these parameters must be indicated:
- **name**: name of device, string type
- **type**: type of device, string type
- **creation_date**: date of creation of row in devices table, string type with YYYY-MM-DD format
- **location_id**: ID of location, integer type
### Request
#### Example
```
http://localhost:8080/devices/1?name=DeviceA&creation_date=2023-07-30
```
### Response
#### Example
```json
{
  "status": "success",
  "message": "Successfully updated device: 1"
}
```
**Fields**
- **status**: status of response
- **message**: message indicating success or cause of error

## DELETE /devices/{serial_number}
- **Description**: deletes a device by serial number
- **Operation**: delete
- **Return**: json containing status and message indicating `200 success` or `404 not found` or `500 error`
### Request
#### Example
```
http://localhost:8080/devices/1
```
### Response
#### Example
```json
{
  "status": "success",
  "message": "Successfully deleted device: 1"
}
```
**Fields**
- **status**: status of response
- **message**: message indicating success or cause of error

## GET /locations
- **Description**: retrieves a list of all locations
- **Operation**: read
- **Return**: json array containing the metadata for each location or `404 not found` when there are no locations found in the locations table
### Request
#### Example
```
http://localhost:8080/locations
```
### Response
#### Example
  ```json
  [
    {
      "id": 1,
      "name": "Location X",
      "type": "Location Type X"
    },
    {
      "id": 2,
      "name": "Location Y",
      "type": "Location Type Y"
    },
    ...
  ]
```
**Fields**
- **id**: ID of location
- **name**: name of location
- **type**: type of location


## POST /locations
- **Description**: creates a new location entry
- **Operation**: create
- **Return**: json containing status and message indicating `200 success` or `400 invalid` or `409 conflict` or `500 error`
### Parameters
- **name**: name of location, string type
- **type**: type of location, string type
- **id** (optional): ID of location, integer type (if not provided, a default ID is assigned)
### Request
#### Example
```
http://localhost:8080/locations?name=LocationX&type=LocationTypeX
```
### Response
#### Example
```json
{
  "status": "success",
  "message": "New location created successfully: LocationX | LocationTypeX"
}
```
**Fields**
- **status**: status of response
- **message**: message indicating success or cause of error

## PATCH /locations/{id}
- **Description**: updates an existing location by ID
- **Operation**: update
- **Return**: json containing status and message indicating `200 success` or `400 invalid` or `404 not found` or `500 error`
### Parameters
At least one of these parameters must be indicated:
- **name**: name of location, string type
- **type**: type of location, string type
### Request
#### Example
```
http://localhost:8080/locations/1?name=LocationA&type=TypeA
```
### Response
#### Example
```json
{
  "status": "success",
  "message": "Successfully updated location: 1"
}
```
**Fields**
- **status**: status of response
- **message**: message indicating success or cause of error

## DELETE /locations/{id}
- **Description**: deletes a location by ID. All devices with this location ID will also be deleted.
- **Operation**: delete
- **Return**: json containing status and message indicating `200 success` or `404 not found` or `500 error`
### Request
#### Example
```
http://localhost:8080/locations/1
```
### Response
#### Example
```json
{
  "status": "success",
  "message": "Successfully deleted location and devices with location id: 1"
}
```
**Fields**
- **status**: status of response
- **message**: message indicating success or cause of error