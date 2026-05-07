#!/usr/bin/env python3
"""
Sprite sheet coordinate extractor
Extracts animation frame coordinates from sprite sheets by detecting non-transparent regions
"""

from PIL import Image
import os

def detect_frames(image_path):
    """
    Detect sprite frames by finding non-transparent regions
    Returns list of (x, y, width, height) tuples
    """
    try:
        img = Image.open(image_path).convert("RGBA")
        width, height = img.size
        pixels = img.load()
        
        # Create a binary mask of non-transparent pixels
        mask = [[False for _ in range(height)] for _ in range(width)]
        
        for x in range(width):
            for y in range(height):
                r, g, b, a = pixels[x, y]
                if a > 0:  # Non-transparent pixel
                    mask[x][y] = True
        
        # Find connected components (frames)
        frames = []
        visited = [[False for _ in range(height)] for _ in range(width)]
        
        for x in range(width):
            for y in range(height):
                if mask[x][y] and not visited[x][y]:
                    # New frame found - perform flood fill
                    frame_pixels = []
                    stack = [(x, y)]
                    
                    while stack:
                        cx, cy = stack.pop()
                        if cx < 0 or cx >= width or cy < 0 or cy >= height:
                            continue
                        if visited[cx][cy] or not mask[cx][cy]:
                            continue
                        
                        visited[cx][cy] = True
                        frame_pixels.append((cx, cy))
                        
                        # Check neighbors
                        stack.append((cx + 1, cy))
                        stack.append((cx - 1, cy))
                        stack.append((cx, cy + 1))
                        stack.append((cx, cy - 1))
                    
                    if frame_pixels:
                        min_x = min(p[0] for p in frame_pixels)
                        max_x = max(p[0] for p in frame_pixels)
                        min_y = min(p[1] for p in frame_pixels)
                        max_y = max(p[1] for p in frame_pixels)
                        
                        frame_width = max_x - min_x + 1
                        frame_height = max_y - min_y + 1
                        
                        # Filter out small noise (minimum 20x20 pixels)
                        if frame_width >= 20 and frame_height >= 20:
                            frames.append((min_x, min_y, frame_width, frame_height))
        
        # Sort frames by Y then X (top to bottom, left to right)
        frames.sort(key=lambda f: (f[1], f[0]))
        
        # Group frames by Y position (animation rows)
        row_threshold = 50  # frames within 50 pixels Y are in same row
        rows = []
        current_row = []
        last_y = -1000
        
        for frame in frames:
            if frame[1] - last_y > row_threshold:
                if current_row:
                    rows.append(sorted(current_row, key=lambda f: f[0]))
                current_row = []
            current_row.append(frame)
            last_y = frame[1]
        
        if current_row:
            rows.append(sorted(current_row, key=lambda f: f[0]))
        
        return frames, (width, height), rows
        
    except Exception as e:
        print(f"Error processing {image_path}: {e}")
        return [], (0, 0), []

def format_cpp_array(frames, array_name):
    """Format frames as C++ array initialization"""
    if not frames:
        return f"static const int {array_name}[4] = {{ 0, 0, 0, 0 }};"
    
    xs = [f[0] for f in frames]
    ys = [f[1] for f in frames]
    ws = [f[2] for f in frames]
    hs = [f[3] for f in frames]
    
    # Pad or truncate to 4 frames
    while len(xs) < 4:
        xs.append(0)
        ys.append(0)
        ws.append(0)
        hs.append(0)
    
    xs = xs[:4]
    ys = ys[:4]
    ws = ws[:4]
    hs = hs[:4]
    
    return f"static const int {array_name}Xs[4] = {{ {', '.join(map(str, xs))} }};\n" \
           f"static const int {array_name}Ys[4] = {{ {', '.join(map(str, ys))} }};\n" \
           f"static const int {array_name}Ws[4] = {{ {', '.join(map(str, ws))} }};\n" \
           f"static const int {array_name}Hs[4] = {{ {', '.join(map(str, hs))} }};"

if __name__ == "__main__":
    sprite_dir = "Sprites/Enemies/"
    
    enemies = [
        ("Mummy.png", "Mummy"),
        ("Zombies.png", "Zombie"),
        ("Martian.png", "Martian")
    ]
    
    print("Sprite Sheet Frame Detection:")
    print("=" * 60)
    
    for filename, name in enemies:
        path = os.path.join(sprite_dir, filename)
        if os.path.exists(path):
            frames, (img_w, img_h), rows = detect_frames(path)
            print(f"\n{name} ({filename}):")
            print(f"  Image size: {img_w}x{img_h}")
            print(f"  Total frames detected: {len(frames)}")
            print(f"  Animation rows detected: {len(rows)}")
            print(f"\n  Animation rows (grouped by Y position):")
            for i, row in enumerate(rows):
                print(f"    Row {i}: {len(row)} frames, Y range: {row[0][1]} - {row[-1][1]}")
                for j, (x, y, w, h) in enumerate(row[:4]):  # Show first 4 frames per row
                    print(f"      Frame {j}: x={x}, y={y}, w={w}, h={h}")
        else:
            print(f"\n{name}: File not found at {path}")
