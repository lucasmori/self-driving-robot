import axios from "axios";

const api = axios.create({
  baseURL: "http://192.168.15.29"

});

export default api;
