import { useSpring, useTransform, MotionValue, type SpringOptions } from "framer-motion";

export function useSmoothTransform(
  value: MotionValue<number>,
  springOptions: SpringOptions,
  transformer: (value: number) => number
): MotionValue<number> {
  return useSpring(useTransform(value, transformer), springOptions);
}
