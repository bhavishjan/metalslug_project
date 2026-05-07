#!/usr/bin/env python3
"""
Crop sprite sheet frames for manual analysis
"""

from PIL import Image
import os

def crop_frames(image_path, output_dir, frames, max_per_row=4):
    """
    Crop frames from sprite sheet and save as individual images
    """
    try:
        img = Image.open(image_path)
        
        for i, (x, y, w, h) in enumerate(frames[:max_per_row]):
            # Crop the frame
            frame = img.crop((x, y, x + w, y + h))
            
            # Save the frame
            output_path = os.path.join(output_dir, f"frame_{i}.png")
            frame.save(output_path)
            print(f"Saved: {output_path} (x={x}, y={y}, w={w}, h={h})")
            
    except Exception as e:
        print(f"Error cropping frames: {e}")

if __name__ == "__main__":
    sprite_dir = "Sprites/Enemies/"
    output_base = "Sprites/Analysis/"
    
    # Martian - crop frames from different rows
    martian_path = os.path.join(sprite_dir, "Martian.png")
    martian_output = os.path.join(output_base, "Martian")
    
    # Sample frames from different rows based on detection output
    martian_frames = [
        # Row 0 - Y~18 (potential walk/stand)
        (10, 18, 49, 43),
        (64, 18, 49, 43),
        (118, 19, 48, 42),
        (171, 19, 48, 42),
        # Row 1 - Y~79 (potential walk/stand)
        (10, 79, 49, 43),
        (64, 79, 50, 43),
        (119, 80, 50, 42),
        (174, 80, 51, 42),
        # Row 2 - Y~140 (potential attack/hurt)
        (10, 140, 44, 44),
        (59, 140, 41, 44),
        (105, 140, 36, 44),
        (146, 140, 28, 44),
        # Row 3 - Y~202 (potential attack)
        (10, 202, 47, 44),
        (62, 202, 44, 44),
        (111, 202, 42, 44),
        (158, 202, 45, 44),
        # Row 9 - Y~831-953 (potential die - large range)
        (10, 831, 48, 43),
        (10, 879, 51, 37),
        (10, 926, 50, 22),
        (10, 953, 52, 63),
        # Row 10 - Y~1018 (potential die)
        (10, 1018, 52, 63),
        (64, 1018, 52, 63),
        (118, 1018, 52, 63),
        (172, 1018, 52, 63),
    ]
    
    os.makedirs(martian_output, exist_ok=True)
    print("Cropping Martian frames...")
    crop_frames(martian_path, martian_output, martian_frames, len(martian_frames))
    
    # Mummy - crop frames
    mummy_path = os.path.join(sprite_dir, "Mummy.png")
    mummy_output = os.path.join(output_base, "Mummy")
    
    # Sample frames from Mummy based on detection (need to run detection first)
    # For now, use estimated coordinates
    mummy_frames = [
        (10, 18, 49, 43),
        (64, 18, 49, 43),
        (118, 19, 48, 42),
        (171, 19, 48, 42),
        (10, 79, 49, 43),
        (64, 79, 50, 43),
    ]
    
    os.makedirs(mummy_output, exist_ok=True)
    print("\nCropping Mummy frames...")
    crop_frames(mummy_path, mummy_output, mummy_frames, len(mummy_frames))
    
    # Zombie - crop frames
    zombie_path = os.path.join(sprite_dir, "Zombies.png")
    zombie_output = os.path.join(output_base, "Zombie")
    
    # Sample frames from Zombie
    zombie_frames = [
        (10, 18, 49, 43),
        (64, 18, 49, 43),
        (118, 19, 48, 42),
        (171, 19, 48, 42),
    ]
    
    os.makedirs(zombie_output, exist_ok=True)
    print("\nCropping Zombie frames...")
    crop_frames(zombie_path, zombie_output, zombie_frames, len(zombie_frames))
    
    print("\nFrames cropped successfully. Check Sprites/Analysis/ to verify animations.")
