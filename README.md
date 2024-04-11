# Running My C++ Application with Docker

1. To build the Docker image, open `canonical_ta` directory and run:

```bash
docker build -t my-cpp-app .
```

2. To run the Docker container:
```bash
docker run -p 8080:8080 my-cpp-app
```

3. To interact with server:
```
http://localhost:8080/
```

## Documentation
Documentation of the REST API is in:
- `device_registry_spec.md`: only success reponses
- `device_registry_spec.yaml`: includes the different error responses

Documentation of database structure is in `DatabaseStructure.md`.
