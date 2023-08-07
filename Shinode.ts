type Sensor = {
  name: string,
  unit: string,
  setup: () => void,
  sense: () => SensorResult
}
type SensorResult = Pick<Sensor, "name" | "unit"> & { measure: string | number }

type Controller = {
  name: string,
  unit: string,
  setup: () => void,
  control: () => ControllerResult
}
type ControllerInput = Pick<Controller, "name" | "unit"> & { measure: string | number }
type ControllerResult = Pick<Controller, "name" | "unit"> & { measure: string | number }

type HostData = {
  polling_interval: number,
  sensors: Pick<Sensor, "name" | "unit">[],
  controls: Pick<Controller, "name" | "unit">[]
}

class Shinode {
  polling_interval: number;
  last_poll = Date.now();

  constructor(
    public device_id: string, 
    public token: string,
    public client: typeof fetch,
    public sensors: Sensor[],
    public controllers: Controller[],
  ) {
    client("https://esp8266-tls.thesebsite.com", {
      headers: {
        Authorization: "Bearer " + token
      }
    })
      .then(res => res.json())
      .then((data: HostData) => {
        this.polling_interval = data.polling_interval;

        if (!this.sensors.every(sensor => data.sensors.some(receivedSensor => {
          return sensor.name === receivedSensor.name
              && sensor.unit === receivedSensor.unit
        }))) throw new Error("Host sensor data does not match Shinode config.")

        if (!this.controllers.every(control => data.controls.some(receivedControl => {
          return control.name === receivedControl.name
              && control.unit === receivedControl.unit
        }))) throw new Error("Host control data does not match Shinode config.")

        this.sense();
      })
  }

  async sense(): Promise<ControllerInput[]> {
    const results = await Promise.all(this.sensors.map(sensor => sensor.sense()));
    const res = await this.client(`https://esp8266-tls.thesebsite.com/${this.device_id}/sense`, {
      method: "POST",
      headers: {
        Authorization: "Bearer " + this.token
      },
      body: JSON.stringify(results)
    })

    return res.json()
  }


  async control(actions: ControllerInput[]) {
    const results = await Promise.all(actions.map(action => this.controllers[action.name].control(action)));
    this.client(`https://esp8266-tls.thesebsite.com/${this.device_id}/control`, {
      method: "POST",
      headers: {
        Authorization: "Bearer " + this.token
      },
      body: JSON.stringify(results)
    });
  }

  async sync() {
    if (Date.now() - this.last_poll > this.polling_interval) {
      const actions = await this.sense();
      await this.control(actions);
    }
  }
}