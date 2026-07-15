import type { Metadata } from "next";
import { Fira_Sans, Fira_Code } from "next/font/google";
import "./globals.css";

// Fira Sans (UI) + Fira Code (data/brand) — a precise, technical pairing for a data console.
// next/font self-hosts them at build time, so there is no runtime font CDN.
const firaSans = Fira_Sans({
  subsets: ["latin"],
  weight: ["400", "500", "600", "700"],
  variable: "--font-sans",
  display: "swap",
});
const firaCode = Fira_Code({
  subsets: ["latin"],
  weight: ["400", "500", "600", "700"],
  variable: "--font-code",
  display: "swap",
});

export const metadata: Metadata = {
  title: "Pulse — KATANA console",
  description: "Live catalog + analytics console for the KATANA Pulse service",
};

// Apply the saved theme before first paint so there is no light/dark flash.
const themeInit = `(function(){try{var t=localStorage.getItem("pulse-theme");if(t)document.documentElement.setAttribute("data-theme",t);}catch(e){}})();`;

export default function RootLayout({ children }: { children: React.ReactNode }) {
  return (
    <html lang="en" className={`${firaSans.variable} ${firaCode.variable}`}>
      <head>
        <script dangerouslySetInnerHTML={{ __html: themeInit }} />
      </head>
      <body>{children}</body>
    </html>
  );
}
