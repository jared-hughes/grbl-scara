# grbl-scara

***
SCARA robot firmware based on [Grbl v0.9](https://github.com/grbl/grbl)

The implemented kinematics allow a 2-arm SCARA design with a linear z-axis (which may be omitted) as in a pen plotter like https://www.instructables.com/Scara-Draw-2D/. 

More information is on the above instructable link or on the base GRBL site.

The way that the coordinate transformation works is by splitting each move command into segments (default 2.0mm long) and performing coordinate transformation formulas on the endpoints of each segment. In this way, motion planning is not perfect and paths are not perfectly curved, but it's a close enough approximation.

Enable SCARA control through `#define SCARA`. Settings can be changed in `grbl/defaults/defaults_scara.h` or through the serial console:

- `$28=120` to set the upper arm length to 120mm
- `$29=120` to set the lower arm length to 120 mm
- `$30=20` to set the minimum x value to 20mm. This is for a straight-line blockage.
- `$31=100` to set the minimum radius value to 10mm (square root of 100). This prevents the arm from doubling back on itself too far.
- `$32=160000` to set the maximum radius value to 400mm (square root of 160000). This can prevent straightening out too far. The default corresponds to no limit besides that imposed by the lengths of the arms.
- `$33=0.001` to set segmentation tolerance to 0.001mm.
- `$34=2.0` to set segment length to 2.0mm.
