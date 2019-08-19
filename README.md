
  ████████ ██       ██ ██ ███████  ███████
 ██░░░░░░ ░██      ░██░██░██░░░░██░██░░░░██
░██       ░██   █  ░██░██░██   ░██░██   ░██
░█████████░██  ███ ░██░██░███████ ░███████
░░░░░░░░██░██ ██░██░██░██░██░░░░  ░██░░░██
       ░██░████ ░░████░██░██      ░██  ░░██
 ████████ ░██░   ░░░██░██░██      ░██   ░░██
░░░░░░░░  ░░       ░░ ░░ ░░       ░░     ░░

    An anti-theft system at the cashier/employee level.

# Implementation Notes:

    The first feature of this system will be to detect "gap theft": when the cashier does not intentionally scan certain products, or covers the barcode. At this moment, integration w/ POS will not be possible, so the system will have to rely on other means.

    Another assumption is that video footage angles will be limited. By using pose estimation, the motion of a cashier swiping/scanning an item will be inferered. A multi-microphone system will use noise cancellation eq'ing to listen for the corresponding beep. If a the audio-feedback is not heard from the register it can be assumed that there is an error, whether unintentionally or maliciously.

## Proposed Algorithm:

    - Cash registers are infered and the angle in respect to camera perspective is calculated (since its mostly right angles).
        - find the angle which has the most semi-horizontal lines
        - find the flow of motion (customers). this is the register line
        - find cashier by face detection of customer cashier (for when cashier 
    - Cashier area is extrapolated as a straight line perpendicular through all lanes.
    - Cashiers are assumed to be standing in the cashier area, and facing relatively perpendicular to the register lane and are facing toward the customer. This angle is used to differentiate between customer and cashier when customer is standing near the cashier area.
    - Actions are quantified: cashier signing on/interacting with POS computer, cashier scanning items, cashier accepts coupons/cash from customer, cashier bagging, assistance from manager/coworker, etc. Any unknown action is registered for later quantification. Items are counted for any given transaction for later reference.
    - Audio feedback (as beeps) are expected to be heard during the motion of cashier scanning a product (with significant timeout/grace period). If there is no audio feedback, store owner is notified so that further measures can be taken. Irrelevant beeps can be safely ignored, and it can be assumed that no to product will be scanned at exactly the same time (two distinct beeps can be extrapolated from the audio)

## Proposed Calibration/Setup:

    - Audio feedback tone:
        - This can be as simple as entering in a recording of a semi-isolated beep (but requires some effort), or it can be as seemless as extrapolating beeps from audio footage (similar repeated tone over time).
    - Cashier area (position and angle):
        - This can be as simple as someone standing at the first and last registers while facing the customer area for that lane, or it can be as seemless as selecting the first and last registers from footage.
    - It can be assumed that video footage will be from IP cameras:
        - Latency can be addressed by producing light or sound that allows the system to synchronize. (Eg: calibration clap, HW blinking light)
        - Redundant camera angles will have to be manually excluded via regions of interest. In later versions, a full 3d-space can be recreated by redundant poses.

## Foreseen Obstacles:

    - false positives that will arise from objects that are difficult to scan, where the operator attempts multiple times to scan an item.
    - this system will be undermined if legitimate barcodes are replaced w/ invalid barcode stickers as POS system might give regular audio feedback.

## Future Features:

    - Distance estimation via microphones for more accurate detection of scanning


# Libraries:

    Uses the [OpenPose](https://github.com/CMU-Perceptual-Computing-Lab/openpose) library, as there is an option for a [commercial license](https://flintbox.com/public/project/47343/). As of now, this is not being used for commercial purposes, even though it is intended to be used in a business concept.

