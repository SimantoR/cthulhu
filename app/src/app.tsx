import React from "react";
import Helmet from "react-helmet";
import clsx from "classnames";

interface Part {
  id: string;
  transform: Aakara.Transform;
}

function resizeCanvasToDisplaySize(canvas: HTMLCanvasElement) {
  // Lookup the size the browser is displaying the canvas in CSS pixels.

  const displayWidth = canvas.parentElement.clientWidth;
  const displayHeight = canvas.parentElement.clientHeight;

  // const displayWidth = canvas.clientWidth;
  // const displayHeight = canvas.clientHeight;

  // Check if the canvas is not the same size.
  const needResize = canvas.width !== displayWidth || canvas.height !== displayHeight;

  if (needResize) {
    // Make the canvas the same size
    canvas.width = displayWidth;
    canvas.height = displayHeight;
  }

  return needResize;
}

function lerpFloat(start: number, end: number, step: number) {
  return start + step * (end - start);
}

const Router: React.FC = function () {
  const canvasRef = React.useRef<HTMLCanvasElement>();

  //#region States
  /* --------------------------------- States --------------------------------- */
  const [app, set_app] = React.useState<Aakara.App>(null);
  const [camera, set_camera] = React.useState<Aakara.Camera>();
  const [orbitControl, set_orbitControl] = React.useState<Aakara.OrbitCameraControl>();
  const [lightRot, set_lightRot] = React.useState<Vec3>({ x: 0, y: 0, z: 45 });
  const [focusPoint, set_focusPoint] = React.useState<Vec3>({ x: 0, y: 0, z: 0 });
  const [parts, set_parts] = React.useState<Part[]>([]);
  const [mouseState, set_mouseState] = React.useState({
    is_pressed: false,
    delta: undefined as Vec2 | undefined,
  });
  const [, updateState] = React.useState<any>();
  const [keyMap, set_keyMap] = React.useState<any>({});
  const [sensitivity, set_sensitivity] = React.useState({
    mouse: 1,
    keyboard: 1,
  });
  const forceUpdate = React.useCallback(() => updateState({}), []);
  //#endregion

  //#region Memo
  /* ---------------------------------- Memo ---------------------------------- */
  const cam_pos = React.useMemo((): Vec3 => {
    if (!camera) return { x: 0, y: 0, z: 0 };

    const cam_transform = camera.getTransform();
    const pos = cam_transform.position;
    cam_transform.delete();

    return pos;
  }, [camera]);

  const isSpinnerVisible = React.useMemo(() => {
    return parts.length === 0;
  }, [parts]);
  //#endregion

  //#region Effects
  /* --------------------------------- Effects -------------------------------- */
  React.useEffect(() => {
    if (!app) loadaakara().then(bootstrap);

    // wasm memory cleanup on unmount
    return () => {
      app && !app.isDeleted() && app.delete();
      camera && !camera.isDeleted() && camera.delete();
      orbitControl && !orbitControl.isDeleted() && orbitControl.delete();
    };
  }, []);

  React.useEffect(() => {
    if (app && camera) requestAnimationFrame(draw);
  }, [app, camera]);

  React.useEffect(() => {
    if (orbitControl) {
      orbitControl.focus = focusPoint;
      orbitControl.orbit(0, 0);
    }
  }, [focusPoint, orbitControl]);
  //#endregion

  const bootstrap = (aakara: typeof Aakara) => {
    if (!canvasRef.current) {
      console.error("Bootstrap:: No canvas found!");
      return;
    }

    const canvas = canvasRef.current;

    // let _app = wasm.init(`#${ref.id}`, ref.width, ref.height);

    let app_inst = new aakara.App(`#${canvas.id}`, canvas.width, canvas.height);
    console.log(app_inst);
    const camera_inst = app_inst.getCamera();
    const light_inst = app_inst.getLight();
    const renderer_inst = app_inst.getRenderer();

    light_inst.setDirection({ x: 1.0, y: 0.0, z: 0.0 });
    renderer_inst.setColor(1.0, 1.0, 1.0);
    light_inst.setRotation(lightRot);

    set_app(app_inst);
    set_camera(camera_inst);
    light_inst.delete();
    renderer_inst.delete();

    set_orbitControl(new aakara.OrbitCameraControl(camera_inst, { x: 0, y: 0, z: 0 }, 10.0));

    const part_callback = function (part: Aakara.Part) {
      const id = part.getId();
      const transform = part.getTransform();

      console.group("Part Information");
      console.log("ID:       ", id);
      console.log("Position: ", transform.position);
      console.log("Rotation: ", transform.rotation);
      console.log("Scale:    ", transform.scale);
      console.groupEnd();

      set_focusPoint(transform.position);

      set_parts((_parts) => {
        if (!_parts) return [{ id, transform }];
        else return [..._parts, { id, transform }];
      });

      console.timeEnd("Part");
    };

    console.time("Part");
    let part_transform = new aakara.Transform(
      { x: 0.0, y: -1.0, z: 10.0 },
      { x: 0.0, y: 0.0, z: 0.0 },
      { x: 0.5, y: 0.5, z: 0.5 }
    );
    app_inst.loadPart("crate.obj", "crate_diffuse.png", part_transform, part_callback);
    part_transform.delete();

    canvas.addEventListener("click", function (ev: MouseEvent) {
      let rect = canvas.getBoundingClientRect();
      let coords = {
        x: ev.clientX - rect.left,
        y: ev.clientY - rect.top,
      };
      console.log(`Mouse click! X: ${coords.x}, Y: ${coords.y}`);
    });

    // window.addEventListener("resize", function () {
    //   let ref = canvasRef.current;
    //   if (resizeCanvasToDisplaySize(ref)) {
    //     _app.renderer.setViewportSize(ref.width, ref.height);
    //   }
    // });

    console.log("Init complete!");
  };

  function onKeyboardDown(ev: any) {
    console.log(ev.key);

    const key: string = ev.key.toLowerCase();

    set_keyMap((kv_map) => {
      kv_map[key] = true;

      return kv_map;
    });
  }

  function onKeyboardUp(ev: any) {
    const key: string = ev.key.toLowerCase();

    set_keyMap((kv_map) => {
      if (key in kv_map) delete kv_map[key];

      return kv_map;
    });
  }

  function handleMouse(ev: React.MouseEvent<HTMLCanvasElement, MouseEvent>) {
    // console.log(`Mouse event triggered: ${ev.type}`);

    console.log(ev.type);

    // if middle mouse is down, trigger
    if (ev.type === "mousedown" && ev.button === 0)
      set_mouseState({ is_pressed: true, delta: { x: ev.movementX, y: ev.movementY } });
    else if (ev.type === "mouseup") set_mouseState({ is_pressed: false, delta: null });
  }

  function handleMouseMovement(ev: React.MouseEvent<HTMLCanvasElement, MouseEvent>) {
    if (mouseState.is_pressed) {
      // const factor = 0.01 * sensitivity.mouse;
      const factor = 1.0;

      const mouseX = ev.movementX * factor;
      const mouseY = ev.movementY * factor;

      const cam_transform = camera.getTransform();

      let _rotation = cam_transform.rotation;
      _rotation.x += mouseY / 2;
      _rotation.y += -mouseX / 2;
      _rotation.z = 0;
      // camera.transform.rotation = _rotation;

      orbitControl.orbit(mouseY, -mouseX);

      set_mouseState({
        ...mouseState,
        delta: {
          x: ev.movementX,
          y: ev.movementY,
        },
      });
    }
  }

  function handleMouseScroll(ev: React.WheelEvent<HTMLCanvasElement>) {
    if (orbitControl) {
      console.log(`X: ${ev.deltaX}, Y: ${ev.deltaY / 100.0}`);
      orbitControl.distance = orbitControl.distance - ev.deltaY / 100.0;
      orbitControl.orbit(0, 0);
    }
  }

  function draw() {
    const ref = canvasRef.current;

    if (!camera) return;

    if (resizeCanvasToDisplaySize(ref)) {
      console.log(`Setting Viewport to: ${ref.width}x${ref.height}`);
      camera.setViewport({ x: ref.width, y: ref.height });
    }

    try {
      /* -------------------------- feed keyboard events -------------------------- */
      const translation: Vec3 = { x: 0, y: 0, z: 0 };

      const factor = 0.1 * sensitivity.keyboard;

      if ("w" in keyMap) translation.z += factor;
      else if ("s" in keyMap) translation.z -= factor;

      if ("a" in keyMap) translation.x += factor;
      else if ("d" in keyMap) translation.x -= factor;

      const camT = camera.getTransform();
      camT.translate(translation);
      camT.delete();

      const count = app.draw();
      if (count > 0) {
        // console.log(`Drawing: ${count} items`);
      }
    } catch (error: any) {
      console.error(error);
      debugger;
    }

    requestAnimationFrame(draw);
    forceUpdate();
  }

  return (
    <React.Fragment>
      <Helmet>
        <title>Cthulhu</title>
      </Helmet>
      <div
        className={clsx(
          "absolute top-0 left-0 w-screen h-screen z-10 bg-black bg-opacity-40 items-center justify-center overflow-hidden",
          {
            "flex flex-row": isSpinnerVisible,
            "hidden absolute": !isSpinnerVisible,
          }
        )}
      >
        <div className="lds-facebook">
          <div />
          <div />
          <div />
        </div>
      </div>
      <div>
        <div className="grid w-screen h-screen grid-cols-12">
          <div className="col-span-3 py-2 bg-white border-r">
            <div className="grid grid-cols-4 px-2 py-4">
              <label>Name</label>
              <input
                aria-label="Item Name"
                className="col-span-3 px-2 border rounded shadow-inner"
                type="text"
                defaultValue="cube crate"
              />
            </div>
            <div className="grid w-full grid-rows-1 gap-4">
              <div className="w-full py-2 text-center text-white bg-gray-700 border">
                <h2 className="font-semibold uppercase font-xl">Transform</h2>
              </div>
              {parts &&
                parts.map((part, i) => (
                  <div key={i} className="px-2">
                    <h3 className="my-2 font-semibold">Position</h3>

                    <div className="grid grid-cols-3 gap-2">
                      <div className="flex flex-row w-full">
                        <div className="flex flex-row items-center px-2 font-bold text-white bg-red-900">
                          <span>X</span>
                        </div>
                        <input
                          aria-label="Position X"
                          type="number"
                          value={part.transform.position.x.toFixed(2)}
                          onChange={(evt) => {
                            let position = part.transform.position;

                            position.x = Number.parseFloat(evt.target.value);
                            part.transform.position = position;
                          }}
                          className="w-full px-2 py-1 border shadow-inner focus:shadow-lg"
                        />
                      </div>
                      <div className="flex flex-row w-full">
                        <div className="flex flex-row items-center px-2 font-bold text-white bg-green-900">
                          <span>Y</span>
                        </div>
                        <input
                          aria-label="Position Y"
                          type="number"
                          value={part.transform.position.y}
                          onChange={(evt) => {
                            const position: Vec3 = part.transform.position;

                            position.y = Number.parseFloat(evt.target.value);
                            part.transform.position = position;
                          }}
                          className="w-full px-2 py-1 border shadow-inner focus:shadow-lg"
                        />
                      </div>
                      <div className="flex flex-row w-full">
                        <div className="flex flex-row items-center px-2 font-bold text-white bg-blue-900">
                          <span>Z</span>
                        </div>
                        <input
                          aria-label="Position Z"
                          type="number"
                          value={part.transform.position.z}
                          onChange={(evt) => {
                            let position = part.transform.position;

                            position.z = Number.parseFloat(evt.target.value);
                            part.transform.position = position;
                          }}
                          className="w-full px-2 py-1 border shadow-inner focus:shadow-lg"
                        />
                      </div>
                    </div>

                    <h3 className="my-2 font-semibold">Rotation</h3>

                    <div className="grid grid-cols-3 gap-2">
                      <div className="flex flex-row w-full">
                        <div className="flex flex-row items-center px-2 font-bold text-white bg-red-900">
                          <span>X</span>
                        </div>
                        <input
                          type="number"
                          value={part.transform.rotation.x}
                          onChange={(evt) => {
                            const transform = part.transform;

                            transform.rotation.x = Number.parseFloat(evt.target.value);

                            part.transform = transform;
                          }}
                          className="w-full px-2 py-1 border shadow-inner focus:shadow-lg"
                        />
                      </div>
                      <div className="flex flex-row w-full">
                        <div className="flex flex-row items-center px-2 font-bold text-white bg-green-900">
                          <span>Y</span>
                        </div>
                        <input
                          type="number"
                          value={part.transform.rotation.y}
                          onChange={(evt) => {
                            const transform = part.transform;

                            transform.rotation.y = Number.parseFloat(evt.target.value);

                            part.transform = transform;
                          }}
                          className="w-full px-2 py-1 border shadow-inner focus:shadow-lg"
                        />
                      </div>
                      <div className="flex flex-row w-full">
                        <div className="flex flex-row items-center px-2 font-bold text-white bg-blue-900">
                          <span>Z</span>
                        </div>
                        <input
                          type="number"
                          value={part.transform.rotation.z}
                          onChange={(evt) => {
                            const transform = part.transform;

                            transform.rotation.z = Number.parseFloat(evt.target.value);

                            part.transform = transform;
                          }}
                          className="w-full px-2 py-1 border shadow-inner focus:shadow-lg"
                        />
                      </div>
                    </div>
                  </div>
                ))}
            </div>

            <div className="w-full my-4 border-b" />

            <div className="grid w-full grid-rows-1 gap-4 p-2">
              <h2 className="font-semibold uppercase font-xl">Camera</h2>
              <div className="flex flex-row w-full">
                <div className="flex flex-row items-center px-4 font-bold text-white bg-red-900">
                  <span>X</span>
                </div>
                <input
                  type="number"
                  value={cam_pos.x}
                  onChange={(evt) => {
                    cam_pos.x = Number.parseFloat(evt.target.value);
                    const camT = camera.getTransform();
                    camT.position = cam_pos;
                    camT.delete();
                    set_camera(camera);
                  }}
                  className="w-full p-2 border shadow-inner focus:shadow-lg"
                />
              </div>
              <div className="flex flex-row w-full">
                <div className="flex flex-row items-center px-4 font-bold text-white bg-green-900">
                  <span>Y</span>
                </div>
                <input
                  type="number"
                  value={cam_pos.y}
                  onChange={(evt) => {
                    cam_pos.y = Number.parseFloat(evt.target.value);
                    const camT = camera.getTransform();
                    camT.position = cam_pos;
                    camT.delete();
                    set_camera(camera);
                  }}
                  className="w-full p-2 border shadow-inner focus:shadow-lg"
                />
              </div>
              <div className="flex flex-row w-full">
                <div className="flex flex-row items-center px-4 font-bold text-white bg-blue-900">
                  <span>Z</span>
                </div>
                <input
                  type="number"
                  value={cam_pos.z}
                  onChange={(evt) => {
                    cam_pos.z = Number.parseFloat(evt.target.value);
                    const camT = camera.getTransform();
                    camT.position = cam_pos;
                    camT.delete();
                    set_camera(camera);
                  }}
                  className="w-full p-2 border shadow-inner focus:shadow-lg"
                />
              </div>
            </div>

            <div className="w-full my-4 border-b" />

            <div className="grid w-full grid-rows-1 gap-4 p-2">
              <h2 className="font-semibold uppercase font-xl">Light Direction</h2>
              <div className="flex flex-row w-full">
                <div className="flex flex-row items-center px-4 font-bold text-white bg-red-900">
                  <span>X</span>
                </div>
                <input
                  type="number"
                  value={lightRot.x}
                  onChange={(evt) => {
                    lightRot.x = Number.parseFloat(evt.target.value);
                    const light = app.getLight();
                    light.setRotation(lightRot);
                    light.delete();
                    set_lightRot(lightRot);
                  }}
                  className="w-full p-2 border shadow-inner focus:shadow-lg"
                />
              </div>
              <div className="flex flex-row w-full">
                <div className="flex flex-row items-center px-4 font-bold text-white bg-green-900">
                  <span>Y</span>
                </div>
                <input
                  type="number"
                  value={lightRot.y}
                  onChange={(evt) => {
                    lightRot.y = Number.parseFloat(evt.target.value);
                    const light = app.getLight();
                    light.setRotation(lightRot);
                    light.delete();
                    set_lightRot(lightRot);
                  }}
                  className="w-full p-2 border shadow-inner focus:shadow-lg"
                />
              </div>
              <div className="flex flex-row w-full">
                <div className="flex flex-row items-center px-4 font-bold text-white bg-blue-900">
                  <span>Z</span>
                </div>
                <input
                  type="number"
                  value={lightRot.z}
                  onChange={(evt) => {
                    lightRot.z = Number.parseFloat(evt.target.value);
                    const light = app.getLight();
                    light.setRotation(lightRot);
                    !light.isDeleted() && light.delete();
                    set_lightRot(lightRot);
                  }}
                  className="w-full p-2 border shadow-inner focus:shadow-lg"
                />
              </div>
            </div>
          </div>
          <div className="col-span-9">
            <div className="relative flex flex-row items-center justify-center w-full h-full">
              <canvas
                tabIndex={0}
                onKeyDown={onKeyboardDown}
                onKeyUp={onKeyboardUp}
                onMouseUp={handleMouse}
                onMouseDown={handleMouse}
                onMouseMove={handleMouseMovement}
                onWheel={handleMouseScroll}
                ref={canvasRef}
                id="kjhasd"
                className="absolute top-0 bottom-0 left-0 right-0 w-full h-full"
              />
            </div>
          </div>
        </div>
      </div>
    </React.Fragment>
  );
};

export default Router;
