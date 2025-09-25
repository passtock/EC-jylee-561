import os
from docx import Document
import markdownify
from fpdf import FPDF

def docx_to_markdown(docx_path, md_path):
    doc = Document(docx_path)
    md_content = ""
    for para in doc.paragraphs:
        md_content += markdownify.markdownify(para.text, heading_style="ATX") + "\n\n"
    with open(md_path, "w", encoding="utf-8") as f:
        f.write(md_content)
    return md_content

def markdown_to_pdf(md_content, pdf_path):
    pdf = FPDF()
    pdf.add_page()
    pdf.set_auto_page_break(auto=True, margin=15)
    pdf.set_font("Arial", size=12)
    for line in md_content.split('\n'):
        pdf.multi_cell(0, 10, line)
    pdf.output(pdf_path)

if __name__ == "__main__":
    docx_file = r"C:\Users\passp\source\repos\EC\report\LAB_GPIO 22000561_leejeayong.docx"
    md_file = r"C:\Users\passp\source\repos\EC\report\LAB_GPIO_22000561_leejeayong.md"
    pdf_file = r"C:\Users\passp\source\repos\EC\report\LAB_GPIO_22000561_leejeayong.pdf"

    md_content = docx_to_markdown(docx_file, md_file)
    markdown_to_pdf(md_content, pdf_file)
    print("변환이 완료되었습니다.")