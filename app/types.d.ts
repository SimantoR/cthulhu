interface Vec2 {
  x: number;
  y: number;
}

interface Vec3 extends Vec2 {
  z: number;
}

module Aakara {
  module FS {
    load();
  }

  class Transform {
    position: Vec3;
    rotation: Vec3;
    scale: Vec3;

    constructor(pos: Vec3, rot: Vec3, scale: Vec3);
    forward(): Vec3;
    up(): Vec3;
    right(): Vec3;
    translate(delta: Vec3): void;
  }

  class Light {
    getStrength(): number;
    setStrength(v: number);
    getColor(): Vec3;
    setColor(c: Vec3);
    getDirection(): Vec3;
    setDirection(d: Vec3);
    setRotation(r: Vec3);
  }

  class Camera {
    transform: Transform;
    getViewport(): Vec2;
    setViewport(xy: Vec2): void;
  }

  class OrbitCameraControl {
    public focus: Vec3;
    public distance: number;

    constructor(camera: Camera, focus: Vec3, distance: number);
    orbit(x: number, y: number);
    getCamera(): Camera;
    setCamera(cam: Camera);
  }

  class Part {
    getId(): string;
    getTransform(): Transform;
  }

  class Renderer {
    setColor(r: number, g: number, b: number);
  }

  class App {
    constructor(id: string, width: number, height: number);
    getRenderer(): Renderer;
    getCamera(): Camera;
    getLight(): Light;
    draw(): number;
    setTransform(id: string, transform: Transform): void;
    loadPart(mesh_url: string, tex_url: string, transform: Transform, cb: (part: Part) => void): void;
    removePart(id: string): void;
  }
}

type AakaraModule = typeof Aakara;

function loadaakara(): Promise<AakaraModule>;
