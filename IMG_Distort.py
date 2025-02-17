import cv2
import numpy as np
import os
import random
import tkinter as tk
from tkinter import filedialog, messagebox

def distort_image(input_path, output_path):
    img = cv2.imread(input_path)
    if img is None:
        return

    random.seed()

    # Random rotation
    if random.choice([True, False]):
        angle = random.randint(0, 359)
        height, width = img.shape[:2]
        center = (width // 2, height // 2)
        rot_mat = cv2.getRotationMatrix2D(center, angle, 1.0)
        img = cv2.warpAffine(img, rot_mat, (width, height))

    # Random blur
    if random.choice([True, False]):
        blur_size = 2 * random.randint(1, 3) + 1  # Always odd
        img = cv2.GaussianBlur(img, (blur_size, blur_size), 0)

    # Random noise
    if random.choice([True, False]):
        noise = np.zeros_like(img, dtype=np.int16)
        cv2.randn(noise, (0, 0, 0), (30, 30, 30))
        img = cv2.add(img.astype(np.int16), noise, dtype=cv2.CV_8U)

    # Random brightness shift
    if random.choice([True, False]):
        brightness = random.randint(-50, 50)
        img = cv2.add(img, np.array([brightness, brightness, brightness], dtype=np.uint8))

    # Random contrast adjustment
    if random.choice([True, False]):
        alpha = 0.5 + random.random()
        img = cv2.convertScaleAbs(img, alpha=alpha, beta=0)

    # Save the distorted image
    cv2.imwrite(output_path, img)

def process_images(input_folder, output_folder):
    for filename in os.listdir(input_folder):
        input_path = os.path.join(input_folder, filename)
        output_path = os.path.join(output_folder, filename)
        if os.path.isfile(input_path):
            distort_image(input_path, output_path)

def select_folder():
    folder = filedialog.askdirectory()
    return folder if folder else None

def on_process_clicked():
    input_folder = select_folder()
    if not input_folder:
        return

    output_folder = select_folder()
    if not output_folder:
        return

    process_images(input_folder, output_folder)
    messagebox.showinfo("Processing Complete", "Image processing complete!")

def main():
    root = tk.Tk()
    root.title("Image Distortion Tool")
    root.geometry("300x100")
    
    process_button = tk.Button(root, text="Select Folders & Process", command=on_process_clicked)
    process_button.pack(pady=20)
    
    root.mainloop()

if __name__ == "__main__":
    main()
