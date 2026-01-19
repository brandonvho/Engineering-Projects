import { MotionConfig, useMotionValue, MotionValue, useSpring } from "framer-motion";
import { useRef, useLayoutEffect, useEffect } from "react";
import { transition } from "./settings";
import { Canvas, useThree, useFrame } from "@react-three/fiber";
import { useSmoothTransform } from "./use-smooth-transform";
import { PerspectiveCamera, Group as ThreeGroup, Mesh as ThreeMesh } from "three";

const spring = { stiffness: 600, damping: 30 };
const mouseToLightRotation = (v: number) => (-1 * v) / 140;

interface ShapesProps {
  isHover: boolean;
  isPress: boolean;
  mouseX: MotionValue<number>;
  mouseY: MotionValue<number>;
}

// Animated group component
function AnimatedGroup({ 
  children, 
  rotation, 
  variants, 
  animate 
}: { 
  children: React.ReactNode;
  rotation?: [MotionValue<number>, MotionValue<number>, number];
  variants?: { hover?: { z?: number } };
  animate?: string;
}) {
  const groupRef = useRef<ThreeGroup>(null);
  const targetZ = variants?.hover && animate === "hover" ? (variants.hover.z ?? 0) : 0;
  const baseZ = useMotionValue(targetZ);
  const zValue = useSpring(baseZ, spring);
  
  useEffect(() => {
    if (variants?.hover && animate === "hover") {
      baseZ.set(variants.hover.z ?? 0);
    } else {
      baseZ.set(0);
    }
  }, [animate, variants, baseZ]);
  
  useFrame(() => {
    if (groupRef.current) {
      if (rotation) {
        groupRef.current.rotation.x = rotation[0].get();
        groupRef.current.rotation.y = rotation[1].get();
        groupRef.current.rotation.z = rotation[2];
      }
      groupRef.current.position.z = zValue.get();
    }
  });
  
  return <group ref={groupRef}>{children}</group>;
}

// Animated mesh component
function AnimatedMesh({
  children,
  position = [0, 0, 0],
  rotation = [0, 0, 0],
  variants,
  animate,
  ...props
}: {
  children: React.ReactNode;
  position?: [number, number, number];
  rotation?: [number, number, number];
  variants?: { hover?: { [key: string]: number } };
  animate?: string;
  [key: string]: any;
}) {
  const meshRef = useRef<ThreeMesh>(null);
  
  // Determine target values based on animate state
  const isHover = animate === "hover";
  const targetX = isHover && variants?.hover?.x !== undefined ? variants.hover.x : position[0];
  const targetY = isHover && variants?.hover?.y !== undefined ? variants.hover.y : position[1];
  const targetZ = isHover && variants?.hover?.z !== undefined ? variants.hover.z : position[2];
  const targetRotateX = isHover && variants?.hover?.rotateX !== undefined ? variants.hover.rotateX : rotation[0];
  const targetRotateY = isHover && variants?.hover?.rotateY !== undefined ? variants.hover.rotateY : rotation[1];
  const targetRotateZ = isHover && variants?.hover?.rotateZ !== undefined ? variants.hover.rotateZ : rotation[2];
  
  // Initialize motion values with target values
  const baseX = useMotionValue(targetX);
  const baseY = useMotionValue(targetY);
  const baseZ = useMotionValue(targetZ);
  const baseRotateX = useMotionValue(targetRotateX);
  const baseRotateY = useMotionValue(targetRotateY);
  const baseRotateZ = useMotionValue(targetRotateZ);
  
  // Apply spring animation
  const x = useSpring(baseX, spring);
  const y = useSpring(baseY, spring);
  const z = useSpring(baseZ, spring);
  const rotateX = useSpring(baseRotateX, spring);
  const rotateY = useSpring(baseRotateY, spring);
  const rotateZ = useSpring(baseRotateZ, spring);
  
  // Update target values when animate state changes
  useEffect(() => {
    if (isHover && variants?.hover) {
      const hover = variants.hover;
      if (hover.x !== undefined) baseX.set(hover.x);
      if (hover.y !== undefined) baseY.set(hover.y);
      if (hover.z !== undefined) baseZ.set(hover.z);
      if (hover.rotateX !== undefined) baseRotateX.set(hover.rotateX);
      if (hover.rotateY !== undefined) baseRotateY.set(hover.rotateY);
      if (hover.rotateZ !== undefined) baseRotateZ.set(hover.rotateZ);
    } else {
      baseX.set(position[0]);
      baseY.set(position[1]);
      baseZ.set(position[2]);
      baseRotateX.set(rotation[0]);
      baseRotateY.set(rotation[1]);
      baseRotateZ.set(rotation[2]);
    }
  }, [animate, variants, position, rotation, baseX, baseY, baseZ, baseRotateX, baseRotateY, baseRotateZ, isHover]);
  
  // Set initial position on mount
  useEffect(() => {
    if (meshRef.current) {
      meshRef.current.position.set(position[0], position[1], position[2]);
      meshRef.current.rotation.set(rotation[0], rotation[1], rotation[2]);
    }
  }, []);
  
  useFrame(() => {
    if (meshRef.current) {
      meshRef.current.position.x = x.get();
      meshRef.current.position.y = y.get();
      meshRef.current.position.z = z.get();
      meshRef.current.rotation.x = rotateX.get();
      meshRef.current.rotation.y = rotateY.get();
      meshRef.current.rotation.z = rotateZ.get();
    }
  });
  
  return (
    <mesh ref={meshRef} {...props}>
      {children}
    </mesh>
  );
}

export function Shapes({ isHover, isPress, mouseX, mouseY }: ShapesProps) {
  const lightRotateX = useSmoothTransform(mouseY, spring, mouseToLightRotation);
  const lightRotateY = useSmoothTransform(mouseX, spring, mouseToLightRotation);

  return (
    <Canvas shadows dpr={[1, 2]} resize={{ scroll: false, offsetSize: true }}>
      <Camera mouseX={mouseX} mouseY={mouseY} />
      <MotionConfig transition={transition}>
        <AnimatedGroup
          rotation={[lightRotateX, lightRotateY, 0]}
        >
          <Lights />
        </AnimatedGroup>
        <AnimatedGroup
          variants={{
            hover: { z: isPress ? -0.9 : 0 }
          }}
          animate={isHover ? "hover" : "rest"}
        >
          <Sphere isHover={isHover} />
          <Cone isHover={isHover} />
          <Torus isHover={isHover} />
          <Icosahedron isHover={isHover} />
        </AnimatedGroup>
      </MotionConfig>
    </Canvas>
  );
}

export function Lights() {
  return (
    <>
      <spotLight color="#61dafb" position={[-10, -10, -10]} intensity={0.2} />
      <spotLight color="#61dafb" position={[-10, 0, 15]} intensity={0.8} />
      <spotLight color="#61dafb" position={[-5, 20, 2]} intensity={0.5} />
      <spotLight color="#f2056f" position={[15, 10, -2]} intensity={2} />
      <spotLight color="#f2056f" position={[15, 10, 5]} intensity={1} />
      <spotLight color="#b107db" position={[5, -10, 5]} intensity={0.8} />
    </>
  );
}

export function Sphere({ isHover }: { isHover: boolean }) {
  return (
    <AnimatedMesh position={[-0.5, -0.5, 0]} variants={{ hover: { z: 2 } }} animate={isHover ? "hover" : "rest"}>
      <sphereGeometry args={[0.4]} />
      <Material />
    </AnimatedMesh>
  );
}

export function Cone({ isHover }: { isHover: boolean }) {
  return (
    <AnimatedMesh
      position={[-0.8, 0.4, 0]}
      rotation={[-0.5, 0, -0.3]}
      variants={{
        hover: {
          z: 1.1,
          x: -1.5,
          rotateX: -0.2,
          rotateZ: 0.4
        }
      }}
      animate={isHover ? "hover" : "rest"}
    >
      <coneGeometry args={[0.3, 0.6, 20]} />
      <Material />
    </AnimatedMesh>
  );
}

export function Torus({ isHover }: { isHover: boolean }) {
  return (
    <AnimatedMesh
      position={[0.1, 0.4, 0]}
      rotation={[-0.5, 0.5, 0]}
      variants={{
        hover: {
          y: 0.5,
          z: 2,
          rotateY: -0.2
        }
      }}
      animate={isHover ? "hover" : "rest"}
    >
      <torusGeometry args={[0.2, 0.1, 10, 50]} />
      <Material />
    </AnimatedMesh>
  );
}

export function Icosahedron({ isHover }: { isHover: boolean }) {
  return (
    <AnimatedMesh
      position={[1.1, 0, 0]}
      rotation={[0, 0, 0.5]}
      variants={{
        hover: {
          x: 1.8,
          z: 0.6,
          y: 0.6,
          rotateZ: -0.5
        }
      }}
      animate={isHover ? "hover" : "rest"}
    >
      <icosahedronGeometry args={[0.7, 0]} />
      <Material />
    </AnimatedMesh>
  );
}

export function Material() {
  return <meshPhongMaterial color="#fff" specular="#61dafb" shininess={10} />;
}

interface CameraProps {
  mouseX: MotionValue<number>;
  mouseY: MotionValue<number>;
}

// Adapted from https://github.com/pmndrs/drei/blob/master/src/core/PerspectiveCamera.tsx
function Camera({ mouseX, mouseY, ...props }: CameraProps) {
  const cameraX = useSmoothTransform(mouseX, spring, (x) => x / 350);
  const cameraY = useSmoothTransform(mouseY, spring, (y) => (-1 * y) / 350);

  const set = useThree(({ set }) => set);
  const camera = useThree(({ camera }) => camera);
  const size = useThree(({ size }) => size);
  const scene = useThree(({ scene }) => scene);
  const cameraRef = useRef<PerspectiveCamera>(null);

  useLayoutEffect(() => {
    const { current: cam } = cameraRef;
    if (cam) {
      cam.aspect = size.width / size.height;
      cam.updateProjectionMatrix();
    }
  }, [size, props]);

  useLayoutEffect(() => {
    if (cameraRef.current) {
      const oldCam = camera;
      set(() => ({ camera: cameraRef.current! }));
      return () => set(() => ({ camera: oldCam }));
    }
  }, [camera, cameraRef, set]);

  useFrame(() => {
    if (cameraRef.current) {
      cameraRef.current.position.x = cameraX.get();
      cameraRef.current.position.y = cameraY.get();
      cameraRef.current.position.z = 3.8;
      camera.lookAt(scene.position);
    }
  });

  return (
    <perspectiveCamera
      ref={cameraRef}
      fov={90}
    />
  );
}

