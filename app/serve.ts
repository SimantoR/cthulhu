import express from "express";

const app = express();
const port = 8080;

app.use(__dirname + "/dist");

app.listen(port, () => console.log("Running on port 8080"));
