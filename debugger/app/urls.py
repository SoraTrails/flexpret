from django.urls import path

from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('inst/<cycle>/', views.instrucion, name='instrucion'),
    path('thread/', views.thread, name='thread'),
]