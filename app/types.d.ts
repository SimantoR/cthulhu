interface Vec2 {
  x: number;
  y: number;
}

interface Vec3 extends Vec2 {
  z: number;
}

class WASMClass {
  delete(): void;
  isDeleted(): boolean;
}

module Aakara {
  namespace FS {
    function open(path: string, flags: any, mode: any): Promise<any>;
    function readFile(path: string, opts: any = {}): Promise<any>;
    function readdir(path: string): Promise<any>;
  }

  class Transform extends WASMClass {
    position: Vec3;
    rotation: Vec3;
    scale: Vec3;

    constructor(pos: Vec3, rot: Vec3, scale: Vec3);
    forward(): Vec3;
    up(): Vec3;
    right(): Vec3;
    translate(delta: Vec3): void;
  }

  interface Shader {
    setInt(loc: string, value: number): void;
    setFloat(loc: string, value: number): void;
    setVector2(loc: string, value: Vec2): void;
    setVector3(loc: string, value: Vec3): void;
  }

  interface Material {
    update(shader: Shader): void;
  }

  class Light extends WASMClass {
    getStrength(): number;
    setStrength(v: number);
    getColor(): Vec3;
    setColor(c: Vec3);
    getDirection(): Vec3;
    setDirection(d: Vec3);
    setRotation(r: Vec3);
  }

  class Camera extends WASMClass {
    getTransform(): Transform;
    getViewport(): Vec2;
    setViewport(xy: Vec2): void;
  }

  class OrbitCameraControl extends WASMClass {
    public focus: Vec3;
    public distance: number;

    constructor(camera: Camera, focus: Vec3, distance: number);
    orbit(x: number, y: number);
    getCamera(): Camera;
    setCamera(cam: Camera);
  }

  class Part extends WASMClass {
    getId(): string;
    getTransform(): Transform;
  }

  class Renderer extends WASMClass {
    setColor(r: number, g: number, b: number);
  }

  class App extends WASMClass {
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
